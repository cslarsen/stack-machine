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
#include <stdarg.h>
#include <vector>
#include "sm.hpp"

const char* to_s(Op op)
{
  switch ( op ) {
  default:  return "<?>"; break;
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
  case PUSH:  return "PUSH"; break;
  case DUP:  return "DUP"; break;
  case SWAP: return "SWAP"; break;
  case ROL3: return "ROL3"; break;
  }
}

fileptr::fileptr(FILE *file) : f(file)
{
  if ( f == NULL ) {
    fprintf(stderr, "Could not open file");
    exit(1);
  }
}

fileptr::~fileptr()
{
  fclose(f);
}

fileptr::operator FILE*() const
{
  return f;
}
