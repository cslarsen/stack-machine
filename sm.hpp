/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * See sm.cpp for more information.
 *
 */

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
