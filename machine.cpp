/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdlib.h>
#include <memory.h>
#include "machine.hpp"
#include "label.hpp"
#include "upper.hpp"

machine_t::machine_t(
  const machine_t& p,
  void (*error_callback)(const char*))
:
  stack(p.stack),
  stackip(p.stackip),
  labels(p.labels),
  memsize(p.memsize),
  memory(new int32_t[p.memsize]),
  ip(p.ip),
  fin(p.fin),
  fout(p.fout),
  running(p.running),
  error_cb(error_callback)
{
  memmove(memory, p.memory, memsize*sizeof(int32_t));
}

machine_t::machine_t(const size_t memory_size,
  FILE* out,
  FILE* in,
  void (*error_callback)(const char*))
:
  stack(),
  stackip(),
  labels(),
  memsize(memory_size),
  memory(new int32_t[memory_size]),
  ip(0),
  fin(in),
  fout(out),
  running(true),
  error_cb(error_callback)
{
  reset();
}

machine_t::machine_t(void (*error_callback)(const char*))
:
  stack(),
  stackip(),
  labels(),
  memsize(1000*1024*sizeof(int32_t)),
  memory(new int32_t[memsize]),
  ip(0),
  fin(stdin),
  fout(stdout),
  running(true),
  error_cb(error_callback)
{
  reset();
}

machine_t& machine_t::operator=(const machine_t& p)
{
  if ( &p == this )
    return *this;

  delete[](memory);

  stack = p.stack;
  stackip = p.stackip;
  labels = p.labels;
  memsize = p.memsize;
  memory = new int32_t[memsize];
  memcpy(memory, p.memory, memsize*sizeof(int32_t));
  ip = p.ip;
  fin = p.fin;
  fout = p.fout;
  running = p.running;
  error_cb = p.error_cb;

  return *this;
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
  if ( error_cb )
    error_cb(s);
}

void machine_t::push(const int32_t& n)
{
  stack.push_back(n);
}

void machine_t::puship(const int32_t& n)
{
  stackip.push_back(n);
}

int32_t machine_t::popip()
{
  if ( stackip.empty() ) {
    error("POP empty IP stack");
    return 0;
  }

  int32_t n = stackip.back();
  stackip.pop_back();
  return n;
}

int32_t machine_t::pop()
{
  if ( stack.empty() )
    error("POP empty stack");

  int32_t n = stack.back();
  stack.pop_back();
  return n;
}

void machine_t::check_bounds(int32_t n, const char* msg) const
{
  if ( n < 0 || static_cast<size_t>(n) >= memsize )
    error(msg);
}

void machine_t::next()
{
  ip += sizeof(int32_t);

  if ( ip < 0 )
    error("IP < 0");

  if ( static_cast<size_t>(ip) >= memsize )
    ip = 0; // TODO: Halt instead of wrap-around?
}

void machine_t::prev()
{
  if ( ip == 0 )
    error("prev() reached zero");

  ip -= sizeof(int32_t);
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
    exec(static_cast<Op>(memory[ip]));

  return 0; // TODO: exit-code ?
}

void machine_t::instr_nop()
{
  next();
}

void machine_t::instr_add()
{
  push(pop() + pop());
  next();
}

void machine_t::instr_sub()
{
  /*
   * This operation is not primitive.  It can
   * be implemented by adding the minuend to
   * the two's complement of the subtrahend:
   *
   * SUB: ; ( a b -- (b-a))
   *   swap  ; b a
   *   compl ; b ~a
   *   1 add ; b (~a+1), or b -a
   *   add   ; b-a
   *   popip
   *
   * The problem is that IF the underlying
   * architecture does not use two's complement
   * to represent negative values, stuff like
   * printing will fail miserably (at least in
   * the current implementation on top of C).
   */

  // TODO: Consider reversing the operands for SUB
  //       (it's currently unnatural)

  int32_t tos = pop();
  push(tos - pop());
  next();
}

void machine_t::instr_and()
{
  push(pop() & pop());
  next();
}

void machine_t::instr_or()
{
  push(pop() | pop());
  next();
}

void machine_t::instr_xor()
{
  push(pop() ^ pop());
  next();
}

void machine_t::instr_not()
{
  // TODO: this probably does not work as intended
  push(!pop());
  next();
}

void machine_t::instr_compl()
{
  push(~pop());
  next();
}

void machine_t::instr_in()
{
  /*
   * The IN/OUT functions should be implemented
   * using something akin to x86 INT or SYSCALL or
   * similar.  E.g.:
   *
   * 123 SYSCALL ; exec system call 123
   *
   */
  push(getc(fin));
  next();
}

void machine_t::instr_out()
{
  putc(pop(), fout);
  fflush(fout);
  next();
}

void machine_t::instr_outnum()
{
  fprintf(fout, "%u", pop());
  next();
}

void machine_t::instr_load()
{
  int32_t a = pop();
  check_bounds(a, "LOAD");
  push(memory[a]);
  next();
}

void machine_t::instr_stor()
{
  int32_t a = pop();
  check_bounds(a, "STOR");
  memory[a] = pop();
  next();
}

void machine_t::instr_jmp()
{
  /*
   * This function is not primitive.
   * If we have e.g. JZ, we can always
   * do "0 JZ" to perform the jump.
   *
   * (Note that this will break the
   * HALT-idiom)
   *
   */

  // TODO: Implement as library function

  //push(0);
  //instr_jz();

  int32_t a = pop();
  check_bounds(a, "JMP");  

  // check if we are halting, i.e. jumping to current
  // address -- if so, quit
  if ( a == ip )
    running = false;
  else
    ip = a;
}

void machine_t::instr_jz()
{
  int32_t a = pop();
  int32_t b = pop();

  if ( a != 0 )
    next();
  else {
    check_bounds(b, "JZ");
    ip = b; // perform jump
  }
}

void machine_t::instr_drop()
{
  pop();
  next();
}

void machine_t::instr_popip()
{
  int32_t a = popip();
  check_bounds(a, "POPIP");
  ip = a;
}

void machine_t::instr_dropip()
{
  popip();
  next();
}

void machine_t::instr_jnz()
{
  /*
   * Only one of JNZ and JZ is needed as
   * a primitive -- one can be implemented
   * in terms of the other with a negation
   * of the TOS.
   *
   * (Note that this will break the HALT-idiom)
   */

  /*
  instr_puship();
  instr_compl();
  instr_popip();
  instr_jz();
  */

  int32_t a = pop();
  int32_t b = pop();

  if ( a == 0 )
    next();
  else {
    check_bounds(b, "JNZ");
    ip = b; // jump
  }
}

void machine_t::instr_push()
{
  next();
  push(memory[ip]);
  next();
}

void machine_t::instr_puship()
{
  next();
  puship(memory[ip]);
  next();
}

void machine_t::instr_dup()
{
  /*
   * This function is not primitive.
   * It can be replaced with a "function":
   *
   * ; ( a -- a a )
   * dup:  nop       ; placeholder <- nop
   *       &dup stor ; placeholder <- a
   *       &dup load ; tos <- a
   *       &dup load ; tos <- a
   *       popip
   */

  // TODO: Implement as library function

  int32_t a = pop();
  push(a);
  push(a);
  next();
}

void machine_t::instr_swap()
{
  /*
   * This function is not primitive.
   * It can be replaced with a "function",
   * something like:
   *
   * ; ( a b -- b a )
   * swap:
   *   swap-b: nop  ; placeholder
   *   swap-a: nop  ; placeholder
   *   &swap-b stor ; swap-b <- b
   *   &swap-a stor ; swap-a <- a
   *   &swap-b load ; tos <- a
   *   &swap-a load ; tos <- b
   *   popip
   *
   */

  // TODO: Implement as library function

  // a, b -- b, a
  int32_t b = pop();
  int32_t a = pop();
  push(b);
  push(a);
  next();
}

void machine_t::instr_rol3()
{
  /*
   * This function is not primitive.
   * It can be replaced with "functions",
   * something like:
   *
   * rol3:
   *   rol3-var: nop  ; stack = a b c
   *   &rol3-var stor ; stack = a b, var = c
   *   swap           ; stack = b a, var = c
   *   &rol3-var load ; stack = b a c
   *   swap           ; stack = b c a
   *   popip
   *
   */

  // TODO: Implement as library function

  // abc -> bca
  int32_t c = pop(); // TOS
  int32_t b = pop();
  int32_t a = pop();
  push(b);
  push(c);
  push(a);
  next();
}

void machine_t::exec(Op operation)
{
  switch(operation) {
  default:     error("Unknown instruction"); break;
  case NOP:    instr_nop();    break;

  // Strictly speaking, SUB can be implemented
  // by ADDing the minuend with the two's complement
  // of the subtrahend -- but that's not necessarily
  // portable down to native code

  case ADD:    instr_add();    break;
  case SUB:    instr_sub();    break; // non-primitive

  // Strictly speaking, all but NOT and AND are
  // non-primitive (or some other combination of
  // two operations)

  case AND:    instr_and();    break;
  case OR:     instr_or();     break;
  case XOR:    instr_xor();    break;
  case NOT:    instr_not();    break;
  case COMPL:  instr_compl();  break;

  // Should be replaced with x86 INT-like operations

  case IN:     instr_in();     break;
  case OUT:    instr_out();    break;

  case LOAD:   instr_load();   break;   
  case STOR:   instr_stor();   break;   

  case PUSH:   instr_push();   break;   
  case DROP:   instr_drop();   break;   

  case PUSHIP: instr_puship(); break; 
  case POPIP:  instr_popip();  break;  
  case DROPIP: instr_dropip(); break; 

  case JZ:     instr_jz();     break;     
  case JMP:    instr_jmp();    break; // non-primitive
  case JNZ:    instr_jnz();    break; // non-primitive
  case DUP:    instr_dup();    break; // non-primitive
  case SWAP:   instr_swap();   break; // non-primitive 
  case ROL3:   instr_rol3();   break; // non-primitive
  case OUTNUM: instr_outnum(); break; // non-primitive
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
    fwrite(start, sizeof(Op), 1, f);
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

void machine_t::addlabel(const char* name, int32_t pos, int)
{
  std::string n = upper(name);

  if ( n.empty() )
    error("Empty label");
  else {
    n.erase(n.length()-1, 1); // remove ":"
    labels.push_back(label_t(n.c_str(), pos));
  }
}

int32_t machine_t::get_label_address(const std::string& s) const
{
  std::string p(upper(s));

  // special label address "here" returns current position
  if ( p == "HERE" )
    return ip;

  for ( size_t n=0; n < labels.size(); ++n )
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
