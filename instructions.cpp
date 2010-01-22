/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdio.h>
#include "instructions.hpp"
#include "machine.hpp"
#include "upper.hpp"

const char* OpStr[] = {
  "NOP",
  "ADD",
  "SUB",
  "AND",
  "OR",
  "XOR",
  "NOT",
  "IN",
  "OUT",
  "LOAD",
  "STOR",
  "JMP",
  "JZ",
  "PUSH",
  "DUP",
  "SWAP",
  "ROL3",
  "OUTNUM",
  "JNZ",
  "DROP",
  "PUSHIP",
  "POPIP",
  "DROPIP",
  "COMPL",
  "NOP_END"
};

const char* to_s(Op op)
{
  if ( op >= NOP && op < NOP_END )
    return OpStr[op];

  return "<?>";
}

Op from_s(const char* str)
{
  std::string s(upper(str));

  // slow, O(n/2) seek... :-)
  for ( int n=0; n<NOP_END; ++n )
    if ( s == OpStr[n] )
      return static_cast<Op>(n);

  return NOP_END;
}
