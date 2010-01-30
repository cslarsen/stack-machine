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
#include <string>

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
  OUTNUM, // pop one byte and write to stream as number
  NOP_END // placeholder for end of enum
};

extern const char* OpStr[];

const char* to_s(Op op);
Op from_s(const char* s);

class fileptr {
  FILE* f;
public:
  fileptr(FILE *file);
  ~fileptr();
  operator FILE*() const;
};

struct label_t {
  std::string name;
  int32_t pos;

  label_t(const char* name_, int32_t position)
    : name(name_), pos(position)
  {
  }
};

struct machine_t {
  std::vector<int32_t> stack;
  std::vector<label_t> labels;
  const size_t memsize;
  int32_t *memory;
  int32_t ip; // instruction pointer
  FILE* fin;
  FILE* fout;
  bool running;

  machine_t(
    const size_t memory_size = 1024*1000/sizeof(int32_t),
    FILE* out = stdout,
    FILE* in  = stdin);

  ~machine_t();
  void reset();
  void error(const char* s) const;
  void push(const int32_t& n);
  int32_t pop();
  void check_bounds(int32_t n, const char* msg);
  void next();
  void load(Op op);
  void load(int32_t n);
  int run(int32_t start_address = 0);
  void eval(Op op);
  int32_t* find_end() const;
  void load_image(FILE* f);
  void save_image(FILE* f) const;
  void load_halt();
  void showstack();
  size_t size() const;
  int32_t cur() const;
  int32_t get_label_address(const char* label) const;
  void addlabel(const char* name, int32_t pos);
};
