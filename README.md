Stack-Machine
=============

This project contains

  * A simple, stack-based virtual machine for executing low-level instructions
  * An assembler supporting a Forth / PostScript like language
  * An interpreter able to run compiled programs

Architecture and design
-----------------------

The instructions are fixed-width at 32-bits and so are the arithmetic
operands.

By default, programs have 1 million cells available for both program text
and data.  This means that a virtual machine memory takes up 4MB plus the
data and instruction stacks.

The text and data regions are overlapped, so you can easily write
self-modifying code (early versions actually required self-modification to
be able to return from subroutine calls, just like Knuth's MIX, but I've
since taken liberty to add such modern convenience into the core instruction
set).

There are no registers.  This _is_ a stack machine, after all.

As we know from theoretical computer science, a pushdown automaton needs
_two_ stacks to be Turing equivalent.  Therefore we employ two as well; one
for the instruction pointer and one for the data.  They live separately from
the text and data region, and are only limited by the host process heap
size.

The machine contains no special facilities besides this:  It's inherently
single-threaded and has no protection mechanisms.  Its operation is
completely sandboxed, though, except for access to standard output.

Aim
---

The project aim was to create a simple machine and language to play around
with.  You can benefit from it by reading the source code, playing with a
language similar to Forth, but conceptually simpler, and finally by seeing
how easy it is to build your own system.

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
with the result of the multiplication.  To run such a program, you'd need to
include the core library first, since multiplication is defined as a
function:

    $ cat tests/core.src your-file.src | sm
    6

Labels, addresses and their values
----------------------------------

Labels are identifiers ending with a colon.

They refer to a particular cell in the machine, and you can access their
position, value or execute code from that cell location:

    label:      ; create a label for the cell at this location
    &label      ; put ADDRESS of label on top of stack
    &label LOAD ; put VALUE of label's cell "label" on top-of-stack
    label       ; EXECUTE code from label position

So, to put the _address_ of a label on the top of the data stack, just
prepend the label name with an ampersand.

If you want the _value_ of an address, put the address on the TOS (top of
stack) and use the `LOAD` instruction to replace the TOS with the value at
the given cell location.

When executing code at a given label position, the machine first puts the
address of the next instruction on top of the instruction stack.  This way
you can return from a function call by using the instruction `POPIP`:

    main:       ; program start
      print-dot
      print-dot
      HALT

    print-dot:
      '.' OUT
      '\n' OUT
      POPIP     ; return from "function"

Variables and subroutines
-------------------------

An idiom for creating variables is to create labels and putting a `NOP` at
that location to reserve one memory cell to hold variables.  An example of
using a counter variable to implement a loop is given below.

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

Notice the use of the `HALT` instruction to stop the program.

Multiplication and core library
-------------------------------

I've implemented a multiplication function in the core library in
`tests/core.src`:

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

    ; ...

    *:        ; alias for mul
      mul
      popip

Note that this function needs definitions for the functions `+` and `-1`.

Recall the program to multiply two numbers.  Put the following in a file
`hey.src`:

    3 2 * outnum
    '\n' out
    halt

If we concatenate the core library with our program, we get:

    $ cat tests/core.src hey.src | ./sm
    6

You could implement the whole program without depending on the core library:

    ; semi-obfuscated multiply and print
    ; does not depend on any libraries

    ; re-inventing the wheel can be very educational!

    main:
      12345 67890 * outnum
      '\n' out
      halt

    ; multiplication function w/inner loop
    *:
      R: nop C: nop N: nop
      &C stor dup &R stor &N stor

      *-loop:
        &R load &N load add &R stor
        1 &C load sub &C stor
        &C load &*-loop swap 1 swap sub jnz

      &R load
      popip

While implementing the Karatsuba algorithm should be quite easy, Toom-Cook
multiplication is left as an exercise for the reader.

It's not a joke
---------------

I think I need to clarify that this project is actually not a joke.  Fun,
absolutely, but not a joke.

I just wanted to create a simple virtual machine and from that I grew a
language.  It's very similar to Forth and PostScript, and we all know those
are extremely powerful --- particularly Forth!

Building stuff yourself is a powerful way of learning.

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

Convenience features
--------------------

I've added a `HALT` instruction.  This replaces the old idiom of looping
forever to signal that a program was finished:

    stop: stop      ; form 1
    stop: &stop jmp ; form 2
    halt            ; convenience form

Originally, it was an argument of minimalism for not including any halt
instructions.

Secondly, I've added a `POPIP` instruction along with automatically storing
the next instruction before performing a jump.  This effectively let's you
call and return from subroutines:

    boot:
      &main jmp halt

    foo: bar: baz:
      '\n' '!' 'e' 'c' 'i' 'u' 'j' 'e' 'l' 't' 'e' 'e' 'B'
      out out out out out out out out out out out out out
      popip

    main:
      foo bar baz

Third, I never bothered to write my own print number function, because it
would require me to write both division and modulus functions in source
first.  So I implemented `OUTNUM` that prints a number to the output:

    123 OUTNUM '\n' OUT ; prints "123\n"

Lacking is proper string handling.  One could say that string handling is
not this language's strongest point.

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

Instruction set
---------------

The instructions are found `include/instructions.hpp`:

    VALUE       OPCODE  EXPLANATION
    0x00000000  NOP     do nothing
    0x00000001  ADD     pop a, pop b, push a + b
    0x00000002  SUB     pop a, pop b, push a - b
    0x00000003  AND     pop a, pop b, push a & b
    0x00000004  OR      pop a, pop b, push a | b
    0x00000005  XOR     pop a, pop b, push a ^ b
    0x00000006  NOT     pop a, push !a
    0x00000007  IN      read one byte from stdin, push as word on stack
    0x00000008  OUT     pop one word and write to stream as one byte
    0x00000009  LOAD    pop a, push word read from address a
    0x0000000A  STOR    pop a, pop b, write b to address a
    0x0000000B  JMP     pop a, goto a
    0x0000000C  JZ      pop a, pop b, if a == 0 goto b
    0x0000000D  PUSH    push next word
    0x0000000E  DUP     duplicate word on stack
    0x0000000F  SWAP    swap top two words on stack
    0x00000010  ROL3    rotate top three words on stack once left, (a b c) -> (b c a)
    0x00000011  OUTNUM  pop one word and write to stream as number
    0x00000012  JNZ     pop a, pop b, if a != 0 goto b
    0x00000013  DROP    remove top of stack
    0x00000014  PUSHIP  push a in IP stack
    0x00000015  POPIP   pop IP stack to current IP, effectively performing a jump
    0x00000016  DROPIP  pop IP, but do not jump
    0x00000017  COMPL   pop a, push the complement of a

The instruction set could easily be more minimal, even more so if we allowed
registers.  Also, we have taken absolutely no care about the machine code
values for each instruction.  A good design would do something cool with
that.

License and author
==================

Placed in the public domain in 2010 by the author, Christian Stigen Larsen
http://csl.sublevel3.org
