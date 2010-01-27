/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * This is a very simple stack machine.
 *
 * It's Turing complete, and you can do anything you want
 * with it, as long as you do it within 1 Mb of memory.
 *
 * Currently, the only program it accepts is hardcoded.
 * Program and memory size will be parameters later on.
 *
 * To stop your programs, just loop forever, as in embedded systems;
 * if you jump to the current line, this is understood as an
 * infinite loop and main() will exit.  The code for that is:
 *
 * load(PUSH); load(ip+sizeof(int32_t)); load(JMP);
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <vector>
#include "sm.hpp"

const char* to_s(Op op)
{
  switch ( op ) {
  default:
  case NOP: return "NOP"; break;
  case ADD: return "ADD"; break;
  case SUB: return "SUB"; break;
  case AND: return "AND"; break;
  case OR:  return "OR"; break;
  case XOR: return "XOR"; break;
  case NOT: return "NOT"; break;
  case IN:  return "IN"; break;
  case OUT: return "OUT"; break;
  case LOAD: return "LOAD"; break;
  case STOR: return "STOR"; break;
  case JMP: return "JMP"; break;
  case JZ:  return "JZ"; break;
  }
}

// machine
std::vector<int32_t> stack;
const size_t memsize = 1024*1000;
int32_t memory[memsize];
int32_t ip = 0; // instruction pointer
FILE* fin = stdin;
FILE* fout = stdout;
const bool debug = true;

static void stop(const char* s, int code=1)
{
  fprintf(stderr, "%s\n", s);
  exit(code);
}

static void push(const int32_t& n)
{
  stack.push_back(n);
}

static int32_t pop()
{
  if ( stack.size() == 0 )
    stop("POP empty stack", 1);

  int32_t n = stack.back();
  stack.pop_back();
  return n;
}

static void check_bounds(int32_t n, const char* msg)
{
  if ( n>=0 && n<memsize )
    return;

  fprintf(stderr, "%s out of bounds\n", msg);
  exit(1);
}

static void help()
{
  Op op=NOP; 
  do {
    printf("0x%x = %s\n", op, to_s(op));
    op = static_cast<Op>(op+1);
  } while ( op != (1+DUP) );

  printf("\nTo halt program, jump to current position:\n\n");
  printf("0x0 PUSH 0x%x\n", sizeof(int32_t));
  printf("0x%x JMP\n\n", sizeof(int32_t));
  printf("Word size is %d bytes\n", sizeof(int32_t));

  exit(0);
}

static void next()
{
  ip += sizeof(int32_t);
  if ( ip >= memsize )
    ip = 0;
}

static void reset()
{
  memset(memory, NOP, sizeof(memory));
  stack.clear();
  ip = 0;
}

static void load(Op op)
{
  memory[ip] = op;
  next();
}

static void load(int32_t n)
{
  memory[ip] = n;
  next();
}

int start(int32_t start_address = 0)
{
  ip = start_address;

  for (;;) {
    Op op = static_cast<Op>(memory[ip]);

    int32_t a=NOP, b=NOP;

    if ( debug ) {
      if ( stack.size() > 1 )
        a = stack[stack.size()-1];
      if ( stack.size() > 2 )
        b = stack[stack.size()-1];

      fprintf(stderr, "ip=%d op=%s stack(%d) = %d, %d\n",
        ip, to_s(op), stack.size(), a, b);
    }

    a=b=NOP;

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
      if ( a == ip ) {
        if ( debug ) stop("HALT", 0);
        exit(0);
      }

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
      a = memory[ip];
      push(a);
      next();
      break;

    case DUP:
      a = pop();
      push(a);
      push(a);
      next();
      break;
    } 
  }
}

class fileptr {
  FILE* f;
public:
  fileptr(const char* path) : f(fopen(path, "rb"))
  {
    if ( f == NULL ) {
      fprintf(stderr, path? path : "<null path>");
      exit(1);
    }
  }

  ~fileptr()
  {
    fclose(f);
  }

  operator FILE*() const
  {
    return f;
  }
};

static void load_file(FILE* f)
{
  reset();

  while ( !feof(f) )
    load(fgetc(f));

  fprintf(stderr, "Read %u bytes", ip);
  ip = 0;
}

int main(int argc, char** argv)
{
  bool found_file = false;
  int code = 0;

  for ( int n=1; n<argc; ++n )
    if ( argv[n][0] == '-' )
      help();
    else {
      found_file = true;

      reset();
      load_file(fileptr(argv[n]));
      code |= start();
    }

  if ( !found_file ) {
    reset();
    load_file(stdin);
    return start();
  } else
    return code;

  return start();
}
