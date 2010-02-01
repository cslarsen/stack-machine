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
#include "sm-core.hpp"
#include "sm-util.hpp"

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
  "NOP_END"
};

const char* to_s(Op op)
{
  if ( op >= NOP && op < NOP_END )
    return OpStr[op];
  return "<?>";
}

Op from_s(const char* s)
{
  int l = strlen(s);
  char* p = static_cast<char*>(malloc(l+1));
  strcpy(p, s);
  for ( char* d=p; *d; ++d )
    if ( *d>='a' && *d<='z' )
      *d = *d - 'a' + 'A';

  // p is now uppercase of s
  for ( int n=0; n<NOP_END; ++n )
    if ( !strcmp(p, OpStr[n]) ) {
      free(p);
      return static_cast<Op>(n);
    }

  free(p);
  return NOP_END;
}

machine_t::machine_t(const size_t memory_size,
  FILE* out,
  FILE* in) :
  memsize(memory_size),
  memory(new int32_t[memory_size]),
  fout(out),
  fin(in),
  running(true)
{
  reset();
}

void machine_t::reset()
{
  memset(memory, NOP, memsize*sizeof(int32_t));
  stack.clear();
  ip = 0;
}

machine_t::~machine_t()
{
  delete[](memory);
}

void machine_t::error(const char* s) const
{
  fprintf(stderr, "%s\n", s);
  exit(1);
}

inline void machine_t::push(const int32_t& n)
{
  stack.push_back(n);
}

inline void machine_t::puship(const int32_t& n)
{
  stackip.push_back(n);
}

inline int32_t machine_t::popip()
{
  if ( stackip.empty() ) {
    error("POP empty IP stack");
    return 0;
  }

  int32_t n = stackip.back();
  stackip.pop_back();
  return n;
}

inline int32_t machine_t::pop()
{
  if ( stack.empty() ) {
    error("POP empty stack");
    return 0;
  }

  int32_t n = stack.back();
  stack.pop_back();
  return n;
}

void machine_t::check_bounds(int32_t n, const char* msg) const
{
  if ( n>=0 && n<memsize )
    return;

  error(msg);
}

void machine_t::next()
{
  ip += sizeof(int32_t);
  if ( ip >= memsize )
    ip = 0;
}

void machine_t::load(Op op)
{
  memory[ip] = op;
  next();
}

void machine_t::load(int32_t n)
{
  memory[ip] = n;
  next();
}

int machine_t::run(int32_t start_address)
{
  ip = start_address;

  while(running)
    eval(static_cast<Op>(memory[ip]));
}

void machine_t::eval(Op op)
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
    push(a - pop());
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
    fflush(fout);
    next();
    break;

  case OUTNUM:
    fprintf(fout, "%u", pop());
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
    check_bounds(a, "STOR");
    memory[a] = pop();
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
    b = pop();

    if ( a != 0 )
      next();
    else {
      check_bounds(b, "JZ");
      ip = b; // jump
    }
    break;

  case DROP:
    pop();
    next();
    break;

  case POPIP:
    a = popip();
    check_bounds(a, "POPIP");
    ip = a;
    break;

  case DROPIP:
    popip();
    next();
    break;

  case JNZ:
    a = pop();
    b = pop();

    if ( a == 0 )
      next();
    else {
      check_bounds(b, "JNZ");
      ip = b; // jump
    }
    break;

  case PUSH:
    next();
    push(memory[ip]);
    next();
    break;

  case PUSHIP:
    next();
    puship(memory[ip]);
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

int32_t* machine_t::find_end() const
{
  // find end of program by scanning
  // backwards until non-NOP is found
  int32_t *p = &memory[memsize-1];
  while ( *p == NOP ) --p;
  return p;
}

void machine_t::load_image(FILE* f)
{
  reset();

  while ( !feof(f) ) {
    Op op = NOP;
    fread(&op, sizeof(Op), 1, f);
    load(op);
  }

  ip = 0;
}

void machine_t::save_image(FILE* f) const
{
  int32_t *start = memory;
  int32_t *end = find_end() + sizeof(int32_t);

  while ( start != end ) {
    int w = fwrite(start, sizeof(Op), 1, f);
    start += sizeof(int32_t);
  }
}

void machine_t::load_halt()
{
  load(PUSH);
  load(ip + sizeof(int32_t));
  load(JMP);
}

size_t machine_t::size() const
{
  return find_end() - &memory[0];
}

int32_t machine_t::cur() const
{
  return memory[ip];
}

int32_t machine_t::pos() const
{
  return ip;
}

void machine_t::addlabel(const char* name, int32_t pos)
{
  std::string n = upper(name);
  n.erase(n.length()-1, 1); // remove ":"
  labels.push_back(label_t(n.c_str(), pos));
}

int32_t machine_t::get_label_address(const char* s) const
{
  std::string p = upper(s);

  // special label address "here" returns current position
  if ( p == "HERE" )
    return ip;

  for ( int n=0; n < labels.size(); ++n )
    if ( upper(labels[n].name.c_str()) == p )
      return labels[n].pos;
  
  return -1; // not found
}

bool machine_t::isrunning() const
{
  return running;
}

void machine_t::set_fout(FILE* f)
{
  fout = f;
}

void machine_t::set_fin(FILE* f)
{
  fin = f;
}

void machine_t::set_mem(int32_t adr, int32_t val)
{
  check_bounds(adr, "set_mem out of bounds");
  memory[adr] = val;
}

int32_t machine_t::get_mem(int32_t adr) const
{
  check_bounds(adr, "get_mem out of bounds");
  return memory[adr];
}

int32_t machine_t::wordsize() const
{
  return sizeof(int32_t);
}
