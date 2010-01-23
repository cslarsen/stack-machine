/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#ifndef INC_SMCORE_H
#define INC_SMCORE_H

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
};

extern const char* OpStr[];

const char* to_s(Op op);
Op from_s(const char* s);

#endif
