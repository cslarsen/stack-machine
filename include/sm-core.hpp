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

struct label_t {
  std::string name;
  int32_t pos;

  label_t(const char* name_, int32_t position)
    : name(name_), pos(position)
  {
  }
};

class machine_t {
  std::vector<int32_t> stack;
  std::vector<int32_t> stackip;
  std::vector<label_t> labels;
  size_t memsize;
  int32_t *memory;
  int32_t ip; // instruction pointer
  FILE* fin;
  FILE* fout;
  bool running;

public:
  machine_t(
    const size_t memory_size = 1024*1000/sizeof(int32_t),
    FILE* out = stdout,
    FILE* in  = stdin);

  machine_t(const machine_t& p) :
    stack(p.stack),
    labels(p.labels),
    memsize(p.memsize),
    memory(new int32_t[p.memsize]),
    ip(p.ip),
    fin(p.fin),
    fout(p.fout),
    running(p.running)
  {
    memmove(memory, p.memory, memsize*sizeof(int32_t));
  }

  machine_t& operator=(const machine_t& p)
  {
    delete [](memory);
    stack = p.stack;
    labels = p.labels;
    memsize = p.memsize;
    memory = new int32_t[p.memsize];
    memmove(memory, p.memory, memsize*sizeof(int32_t));
    ip = p.ip;
    fin = p.fin;
    fout = p.fout;
    running = p.running;
    return *this;
  }

  ~machine_t();
  void reset();
  void error(const char* s) const;
  void push(const int32_t& n);
  int32_t pop();
  void puship(const int32_t&);
  int32_t popip();
  void check_bounds(int32_t n, const char* msg) const; 
  void next();
  void prev();
  void load(Op);
  void load(int32_t n);
  int run(int32_t start_address = 0);
  void exec(Op);
  int32_t* find_end() const;
  void load_image(FILE* f);
  void save_image(FILE* f) const;
  void load_halt();
  void showstack() const;

  size_t size() const;
  int32_t cur() const;
  int32_t pos() const;

  int32_t get_label_address(const char* label) const;
  void addlabel(const char* name, int32_t pos);

  bool isrunning() const;
  void set_fout(FILE*);
  void set_fin(FILE*);

  void set_mem(int32_t adr, int32_t val);
  int32_t get_mem(int32_t adr) const;
  int32_t wordsize() const;

  // instructions
  void instr_nop();    
  void instr_add();    
  void instr_sub();    
  void instr_and();    
  void instr_or();     
  void instr_xor();    
  void instr_not();    
  void instr_in();     
  void instr_out();    
  void instr_outnum(); 
  void instr_load();   
  void instr_stor();   
  void instr_jmp();    
  void instr_jz();     
  void instr_drop();   
  void instr_popip();  
  void instr_dropip(); 
  void instr_jnz();    
  void instr_push();   
  void instr_puship();  
  void instr_dup();    
  void instr_swap();   
  void instr_rol3();   
  void instr_compl();
};

#endif
