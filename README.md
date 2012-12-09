Stack-Machine
=============

This project contains

  * A simple, stack-based virtual machine for executing low-level instructions
  * An assembler supporting a Forth/PostScript-like language
  * An interpreter able to run compiled programs

The instructions are fixed-width at 32-bits and so are the arithmetic
operands.

By default, programs have 1 million cells available for both program text
and data.  This means that a virtual machine memory takes up 4MB plus the
data and instruction stacks.

The text and data regions are overlapped, so you can easily write
self-modifying code (early versions actually required self-modification to
be able to return from subroutine calls, just like Knuth's MIX, but I've
since taken liberty to add such modern convenience).

There are two stacks; one for the instruction pointer and one for the data.
They live separately from the text and data region, and are only limited by
the host process heap size.

The machine contains no special facilities besides this:  It's inherently
single-threaded and has absolutely no concept of execution protection
(though, it's fairly well sandboxed from your host system).

The project aim was to create a simple machine and language to play around
with.

The programming language
========================

The language is very similar to Forth and PostScript:  You basically write
in RPN --- reverse Polish notation.  Anything not recognized as an
instruction is put on the data stack, so to put the numbers 3 and 2 on the
stack, just write

    3 2

To multiply them, just append with an asterix:

    3 2 * ; multiplication

This operation pops the topmost two numbers on the stack and replaces them
with the result of the multiplication.

Except, there is no multiplication function available in the core language.
But I've implemented one for you in `tests/core.src`, with a function called
`mul` (aliased to `*`):

    mul:            ; ( a b -- (a*b) )
      mul-res: nop  ; placeholder for result
      mul-cnt: nop  ; placeholder for counter
      mul-num: nop

      &mul-cnt stor ; b to cnt
      dup
      &mul-res stor ; a to res
      &mul-num stor ; and to num

      mul-loop:
        ; calculate res += a
        &mul-res load
        &mul-num load +
        &mul-res stor

        ; decrement counter
        &mul-cnt load
        -1
        &mul-cnt stor

        ; loop until counter is zero
        &mul-cnt load
        &mul-loop swap -1 jnz

      &mul-res load
      popip

So, to create a program to multiply two numbers, put the following into a
file `hey.src`:

    3 2 * outnum
    '\n' out
    halt

Since the core library is in `tests/core.src` we have to load if before the
main program:

    $ cat tests/core.src hey.src | ./sm
    6

It's not a joke
---------------

I think I need to clarify that this project actually is not a joke.  Fun,
yes, but a joke it's not.  I just wanted to create a simple virtual machine
and from that I grew a language.  It's very similar to Forth and PostScript,
and we all know those are extremely powerful (especially Forth).

But the fact is that subroutines don't really exist, and neither does local
variables, but they can be both be *implemented* with a bit of supporting
code.

Although the machine and stack have limited memory, the language itself is
Turing complete.

Hello, world!
-------------

You can do `72 OUT` to print the letter "H" (72 is the ASCII code for "H").
Cutting to the chase, a program to print "Hello!" would be:

    ; Labels are written as a name without whitespace
    ; and a colon at the end.

    main:
       72 out          ; "H"
      101 out          ; "e"
      108 dup out out  ; "ll"
      111 out          ; "o"
       33 out          ; "!"

      ; newline
      '\n' out

      42 outnum     ; print a number
      '\n' out      ; and newline

      ; stop program
      halt

Notice the use of the `HALT` instruction to stop the program.  Earlier
versions had no such operation.  Back then, an idiom to halt the machine was
to just loop forever:

    do-nothing: &do-nothing JMP

Labels, addresses and their values
----------------------------------

Labels are identifiers ending with a colon.  To put the _address_ of a label
on the top of the data stack, just prepend the label name with an ampersand.
If you want the _value_ of an address, use the `LOAD` instruction.

An idiom in this language is to create labels, put a `NOP` ("no operation"
instruction) there and use that location to store variables.

So, we could create a variable by doing so:

    counter: NOP                     ; reserve 1 word for the variable "counter"

    program: 2 &counter STOR                       ; set counter to two
             &counter LOAD 1 ADD &counter STOR     ; increment counter by one

    ; loop counter+1 times

    display: '\n' '*' OUT OUT                      ; print an asterix
             1 &counter LOAD SUB &counter STOR     ; decrement counter by one
             &display &counter LOAD JNZ            ; jump to display if not zero

The output of the above program is three stars:

    $ ./sm foo.src
    *
    *
    *

You can forward-reference labels.  In fact, another idiom is to jump to the
main part of the program at the start of the source.

Subroutines
-----------

If you reference a label without the ampersand, the virtual machine will
execute the code at that location.  Before changing the instruction pointer,
the next address will be placed on the top of the instruction stack.  You
can therefore use `POPIP` to pop this number off the stack and assign it to
the current instruction pointer.  

In other words, `POPIP` returns from a subroutine.  Or whatever.

So, in the above example, we could create a subroutine `decrement-counter`
like so:

    decrement-counter:  &counter LOAD   ; read variable
                        1 SWAP SUB      ; decrement value
                        &counter STOR   ; update variable
                        POPIP           ; return from "subroutine"

A Fibonacci program
-------------------

The following is a program to generate and print Fibonacci numbers, taken
from `tests/fib.src`:

    ; Print the well-known Fibonacci sequence
    ;
    ; Our word size is only 32-bits, so we can't
    ; count very far.

    ; Program starts at main, so jump there

    &main jmp

    ; Create label 'count', which refers to this memory
    ; address.
    ;
    ; The NOP (no operation; do nothing) is only used
    ; to reserve memory space for a variable.

    count:
      nop

    ; Initialize the counter by storing 46 at the address of 'count'.
    ;
    ; POPIP will pop the instruction pointer, effectively jumping to
    ; the next location (probably the caller).

    count-init:
      46 &count stor
      popip

    ; Shorthand for loading the number at 'count' onto the top of the stack.
    ;
    ; The "( -- counter)" comment is similar to Forth's comments, explaining
    ; that no number is expected on the stack, and after running this function,
    ; a number ("counter") will be on the stack.

    count-get: ; ( -- counter )
      &count load     ; load number
      popip

    ; Shorthand for decrementing the number on the stack

    dec: ; ( a -- a-1 )
      1 swap sub
      popip

    ; Store top of stack to 'count', do not alter stack

    count-set: ; ( counter -- counter )
      dup &count stor
      popip

    ; Decrement counter and return it

    count-dec: ; ( -- counter )
      count-get dec
      count-set
      popip

    ; Print number with a newline without altering stack

    show: ; ( number -- number )
      dup outnum
      '\n' out
      popip

    ; Duplicate two top-most numbers on stack

    dup2: ; ( a b -- a b a b )
      swap       ; b a
      dup        ; b a a
      rol3       ; a a b
      dup        ; a a b b
      rol3       ; a b b a
      swap       ; a b a b
      popip

    jump-if-nonzero: ; ( dest_address predicate -- )
      swap jnz
      popip

    ; The start of our Fibonacci printing program

    main:
      count-init

      0 show  ; first Fibonacci number
      1       ; second Fibonacci number

      loop:
        ; add top numbers and show
        ; a b -> a b a b -> a b (a + b)
        dup2 add show

        ; decrement, loop if non-zero
        count-dec &loop jump-if-nonzero

Instruction set
---------------

The instructions are found `include/instructions.hpp`:

    NOP,  // do nothing
    ADD,  // pop a, pop b, push a + b
    SUB,  // pop a, pop b, push a - b
    AND,  // pop a, pop b, push a & b
    OR,   // pop a, pop b, push a | b
    XOR,  // pop a, pop b, push a ^ b
    NOT,  // pop a, push !a
    IN,   // push one byte read from stream
    OUT,  // pop one byte and write to stream
    LOAD, // pop a, push byte read from address a
    STOR, // pop a, pop b, write b to address a
    JMP,  // pop a, goto a
    JZ,   // pop a, pop b, if a == 0 goto b
    PUSH, // push next word
    DUP,  // duplicate word on stack
    SWAP, // swap top two words on stack
    ROL3, // rotate top three words on stack once left, (a b c) -> (b c a)
    OUTNUM, // pop one byte and write to stream as number
    JNZ,  // pop a, pop b, if a != 0 goto b
    DROP, // remove top of stack
    PUSHIP, // push a in IP stack
    POPIP,  // pop IP stack to current IP, effectively performing a jump
    DROPIP, // pop IP, but do not jump
    COMPL,  // pop a, push the complement of a
    NOP_END // placeholder for end of enum; MUST BE LAST

Compiling the project
=====================

To compile and run the examples:

    $ make all check

To see the low-level machine instructions:

    $ ./smr -h

To execute source code on-the-fly:

    $ ./sm filename

To compile source to bytecode:

    $ ./smc filename

The assembly language is not documented other than in code, because I'm
actively playing with it.

Although the interpreter is slow, it should be possible to convert stack
operations to a register machine.  In fact, it should be trivial to compile
programs to native machine code, e.g. x86.

License and author
==================

Placed in the public domain in 2010 by the author, Christian Stigen Larsen
http://csl.sublevel3.org
