/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdlib.h>
#include "compiler.hpp"
#include "parser.hpp"
#include "machine.hpp"
#include "label.hpp"
#include "upper.hpp"

void compiler::error(const std::string& s)
{
  if ( callback )
    callback(s.c_str());
}

bool compiler::islabel(const std::string& s)
{
  size_t l = s.length();
  return l<1? false : s[l-1] == ':';
}

bool compiler::iscomment(const std::string& s)
{
  return s[0] == ';';
}

Op compiler::tok2op(const std::string& s)
{
  return from_s(s.c_str());
}

bool compiler::isliteral(const std::string& s)
{
  if ( islabel(s) )
    return false;

  return tok2op(s) == NOP_END;
}

bool compiler::isnumber(const char* s)
{
  while ( *s )
    if ( !isdigit(*s++) )
      return false;

  return true;
}

bool compiler::ischar(const std::string& s)
{
  size_t l = s.length();

  if ( l==3 && s[0]=='\'' && s[2]=='\'' && s[1]!='\\' )
    return true;

  if ( l==4 && s[0]=='\'' && s[3]=='\'' && s[1]=='\\' 
            && (s[2]=='t' || s[2]=='r' || s[2]=='n' || s[2]=='0') )
    return true;

  return false;
}

char compiler::to_ord(const std::string& s)
{
  size_t l = s.length();

  if ( l == 3 ) // 'x'
    return s[1];
 
  if ( l == 4 ) // '\x'
    switch ( s[2] ) {
    case 't': return '\t';
    case 'r': return '\r';
    case 'n': return '\n';
    case '0': return '\0';
    }

  error("Unknown character literal: " + s);
  return '\0';
}

bool compiler::islabel_ref(const std::string& s)
{
  return s[0] == '&';
}

int32_t compiler::to_literal(const std::string& s)
{
  if ( isnumber(s.c_str()) )
    return atoi(s.c_str());

  if ( ischar(s) )
    return to_ord(s);

  return -1;
}

bool compiler::ishalt(const std::string& s)
{
  return s.empty() || upper(s)=="HALT";
}

void compiler::check_label_name(const std::string& label)
{
  if ( upper(label) == "HERE" )
    error("Label is reserved: HERE");
}

compiler::compiler(void (*cb)(const char*)) :
  m(cb),
  forwards(),
  callback(cb)
{
}

void compiler::set_error_callback(void (*error_callback)(const char* message))
{
  callback = error_callback;
}

void compiler::compile_label(const std::string& label)
{
  int32_t address = m.get_label_address(label);

  m.load(PUSH);

  // if label not found, mark it for update
  if ( address == -1 ) {
    check_label_name(label);
    forwards.push_back(label_t(label, m.pos()));
  }

  m.load(address);
}

void compiler::compile_function_call(const std::string& function)
{
  // Return address is here plus four instructions
  m.load(PUSHIP); m.load(m.pos() + 4*m.wordsize());

  // Push function destination address -- update it later
  m.load(PUSH);
  forwards.push_back(label_t(function, m.pos()));
  m.load(-1); // just push an arbitrary number

  // Jump to function
  m.load(JMP);

  // This is the return point
}

void compiler::compile_literal(const std::string& token)
{
  if ( islabel_ref(token) ) {
    compile_label(token.substr(1));
    return;
  }

  int32_t literal = to_literal(token);

  // Literals are pushed on to the stack
  if ( literal != -1 ) {
    m.load(PUSH);
    m.load(literal);
    return;
  }

  // Unknown literals are treated as forward function calls
  compile_function_call(token);
}

void compiler::resolve_forwards()
{
  for ( size_t n=0; n<forwards.size(); ++n ) {
    std::string label = forwards[n].name;
    int32_t address = m.get_label_address(label);

    if ( address == -1 )
      error("Code label not found: " + label);

    // update label jump to address
    m.set_mem(forwards[n].pos, address);
  }
}

// Return FALSE when compilation has finished
bool compiler::compile_token(const std::string& s, parser& p)
{
  if ( s.empty() ) {
    m.load_halt();
    resolve_forwards();
    return false;
  }
  else if ( ishalt(s) )    m.load_halt();
  else if ( iscomment(s) ) p.skip_line();
  else if ( isliteral(s) ) compile_literal(s);
  else if ( islabel(s) )   m.addlabel(s.c_str(), m.pos());
  else {
    Op op = tok2op(s);

    if ( op == NOP_END )
      error("Unknown operation: " + s);

    m.load(op);
  }

  return true;
}

machine_t& compiler::get_program()
{
  return m;
}

compiler::compiler(parser& p, void (*fp)(const char*)) :
  m(fp), forwards(), callback(fp)
{
  // Perform complete compilation
  while ( compile_token(p.next_token(), p) )
    ; // loop
}
