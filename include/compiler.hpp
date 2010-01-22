/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include "instructions.hpp"
#include "parser.hpp"
#include "machine.hpp"

#ifndef INC_COMPILER_HPP
#define INC_COMPILER_HPP

class compiler
{
  machine_t m;
  std::vector<label_t> forwards;
  void (*callback)(const char*);

  void error(const std::string& s);
  char to_ord(const std::string& s);
  int32_t to_literal(const std::string& s);
  void check_label_name(const std::string& label);

  static bool islabel(const std::string& s);
  static bool iscomment(const std::string& s);
  static Op tok2op(const std::string& s);
  static bool isliteral(const std::string& s);
  static bool isnumber(const char* s);
  static bool ischar(const std::string& s);
  static bool islabel_ref(const std::string& s);
  static bool ishalt(const std::string& s);

public:
  compiler(void (*error_callback)(const char* message) = NULL);
  compiler(parser& p, void (*error_callback)(const char* message) = NULL);

  void set_error_callback(void (*error_callback)(const char* message));
  void compile_label(const std::string& label);
  void compile_function_call(const std::string& function);
  void compile_literal(const std::string& token);
  void resolve_forwards();
  bool compile_token(const std::string& s, parser& p);
  machine_t& get_program();
};

#endif
