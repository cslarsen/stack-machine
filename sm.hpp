/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * See sm.cpp for more information.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <stdarg.h>
#include <vector>

enum Op {
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
  JZ,   // pop a, if a == 0 goto a
  PUSH, // push next word
  DUP,  // duplicate word on stack
  SWAP, // swap top two words on stack
  ROL3, // rotate top three words on stack once left, (a b c) -> (b c a)
  NOP_END // placeholder for end of enum
};

const char* to_s(Op op);

class fileptr {
  FILE* f;
public:
  fileptr(FILE *file);
  ~fileptr();
  operator FILE*() const;
};

struct machine_t {
  std::vector<int32_t> stack;
  const size_t memsize;
  int32_t *memory;
  int32_t ip; // instruction pointer
  FILE* fin;
  FILE* fout;
  bool running;

  machine_t(
    const size_t memory_size = 1024*1000/sizeof(int32_t),
    FILE* out = stdout,
    FILE* in  = stdin
    ) :
    memsize(memory_size),
    memory(new int32_t[memory_size]),
    fout(out),
    fin(in),
    running(true)
  {
    reset();
  }

  void reset()
  {
    memset(memory, NOP, memsize*sizeof(int32_t));
    stack.clear();
    ip = 0;
  }

  ~machine_t()
  {
    delete[](memory);
  }

  void error(const char* s) const
  {
    fprintf(stderr, "%s\n", s);
    // now what?
    // - push(ip), jump to specified address, pop on return
    // - stop executing
    // - throw
  }

  void push(const int32_t& n)
  {
    stack.push_back(n);
  }

  int32_t pop()
  {
    if ( stack.size() == 0 ) {
      error("POP empty stack");
      return 0;
    }

    int32_t n = stack.back();
    stack.pop_back();
    return n;
  }

  void check_bounds(int32_t n, const char* msg)
  {
    if ( n>=0 && n<memsize )
      return;

    error(msg);
  }

  void next()
  {
    ip += sizeof(int32_t);
    if ( ip >= memsize )
      ip = 0;
  }

  void load(Op op)
  {
    memory[ip] = op;
    next();
  }

  void load(int32_t n)
  {
    memory[ip] = n;
    next();
  }

  int run(int32_t start_address = 0)
  {
    ip = start_address;

    while(running)
      eval(static_cast<Op>(memory[ip]));
  }

  void eval(Op op)
  {
    int32_t a=NOP, b=NOP, c=NOP;

    switch(op) {
    case NOP:
      next();
      break;

    case ADD:
      push(pop() + pop());
      next();
      break;

    case SUB:
      a = pop();
      b = pop();
      push(a-b);
      next();
      break;

    case AND:
      push(pop() & pop());
      next();
      break;

    case OR:
      push(pop() | pop());
      next();
      break;

    case XOR:
      push(pop() ^ pop());
      next();
      break;

    case NOT:
      push(!pop());
      next();
      break;

    case IN:
      push(getc(fin));
      next();
      break;

    case OUT:
      putc(pop(), fout);
      next();
      break;

    case LOAD:
      a = pop();
      check_bounds(a, "LOAD");
      push(memory[a]);
      next();
      break;

    case STOR:
      a = pop();
      b = pop();
      check_bounds(a, "STOR");
      memory[a] = b;
      next();
      break;

    case JMP:
      a = pop();
      check_bounds(a, "JMP");  

      // check if we are halting, i.e. jumping to current
      // address -- if so, quit
      if ( a == ip )
        running = false;
      else
        ip = a;

      break;

    case JZ:
      a = pop();

      if ( a != 0 )
        next();
      else {
        check_bounds(a, "JZ");
        ip = a; // jump
      }
      break;

    case PUSH:
      next();
      push(memory[ip]);
      next();
      break;

    case DUP:
      a = pop();
      push(a);
      push(a);
      next();
      break;

    case SWAP: // a, b -- b, a
      b = pop();
      a = pop();
      push(b);
      push(a);
      next();
      break;

    case ROL3: // abc -> bca
      c = pop();
      b = pop();
      a = pop();
      push(b);
      push(c);
      push(a);
      next();
      break;
    }
  }

  int32_t* find_end()
  {
    // find end of program by scanning
    // backwards until non-NOP is found
    int32_t *p = &memory[memsize-1];
    while ( *p == NOP ) --p;
    return p;
  }

  void load_image(FILE* f)
  {
    reset();

    while ( !feof(f) ) {
      Op op = NOP;
      fread(&op, sizeof(Op), 1, f);
      load(op);
    }

    ip = 0;
  }

  void save_image(FILE* f)
  {
    int32_t *start = memory;
    int32_t *end = find_end() + sizeof(int32_t);

    while ( start != end ) {
      int w = fwrite(start, sizeof(Op), 1, f);
      start += sizeof(int32_t);
    }
  }
};
