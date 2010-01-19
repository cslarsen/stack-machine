/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <string>

#ifndef INC_PARSER_HPP
#define INC_PARSER_HPP

class parser
{
  FILE* f;
  int lineno;
  int update_lineno(int c);
  int fgetchar();
  void move_back(int c);
  void skip_whitespace();

public:
  parser(FILE* f);
  int get_lineno() const;
  std::string next_token();
  void skip_line();
};

#endif
