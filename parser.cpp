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
  while ( (c = fgetchar()) != EOF && isspace(c) );
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

const char* parser::next_token()
{
  // TODO: Fix maximum size of identifiers is 255 characters.
  static char tok[256];

  char* p = &tok[0];
  int c;

  tok[0] = '\0';

  if ( feof(f) )
    return NULL;

  skip_whitespace();

  while ( (c = fgetchar()) != EOF
      && !isspace(c)
      && p-tok<sizeof(tok) )
  {
      *p++ = c;
  }

  *p = '\0';
  return tok;
}

void parser::skip_line()
{
  int c;
  while ( (c = fgetchar()) != EOF && c != '\n' )
    ; // loop
}
