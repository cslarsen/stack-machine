/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdio.h>
#include <vector>
#include <string>
#include "instructions.hpp"
#include "label.hpp"

#ifndef INC_MACHINE_HPP
#define INC_MACHINE_HPP

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
  void (*error_cb)(const char*);

public:
  machine_t(void (*error_callback)(const char* msg));
  machine_t(
    const size_t memory_size = 1024*1000/sizeof(int32_t),
    FILE* out = stdout,
    FILE* in  = stdin,
    void (*error_callback)(const char* msg) = NULL);
  machine_t(const machine_t& p, void (*error_callback)(const char* msg) = NULL);
  machine_t& operator=(const machine_t& p);
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

  int32_t get_label_address(const std::string& label) const;
  void addlabel(const char* name, int32_t pos, int lineno = -1);

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
