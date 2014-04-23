/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include "parser.hpp"

int parser::update_lineno(int c)
{
  if ( c == '\n' )
    ++lineno;

  return c;
}

int parser::fgetchar()
{
  return update_lineno(fgetc(f));
}

void parser::move_back(int c)
{
  if ( c == '\n' )
    --lineno;

  ungetc(c, f);
}

void parser::skip_whitespace()
{
  int c;
  while ( (c = fgetchar()) != EOF && isspace(c) )
    ;
  move_back(c);
}

parser::parser(FILE* file) :
  f(file),
  lineno(1)
{
}

int parser::get_lineno() const
{
  return lineno;
}

std::string parser::next_token()
{
  int c;
  std::string s;

  skip_whitespace();

  while ( (c = fgetchar()) != EOF && !isspace(c) )
      s += c;

  return s;
}

void parser::skip_line()
{
  int c;
  while ( (c = fgetchar()) != EOF && c != '\n' )
    ;
}
