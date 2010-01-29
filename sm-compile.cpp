#include <string>
#include <vector>
#include "sm-core.hpp"

static size_t lineno = 1;

static const char WHITESPACE[] = "\r\n\t ";

static int fgetchar(FILE* f)
{
  int n = fgetc(f);
  if ( n=='\n' ) ++lineno;
  return n;
}

static bool char_in(char ch, const char* in)
{
  while ( *in )
    if ( *in++ == ch )
      return true;
  return false;
}

static void skip(FILE* f, const char* chars)
{
  int ch;

  while ( (ch = fgetchar(f)) != EOF
    && char_in(ch, chars) )
      ; // loop

  ungetc(ch, f);
}

static void skipto(FILE* f, const char* chars)
{
  int ch;

  while ( (ch = fgetchar(f)) != EOF
    && !char_in(ch, chars) )
      ; // loop

  ungetc(ch, f);
}

static void skipws(FILE* f)
{
  skip(f, WHITESPACE);
}

static const char* token(FILE* f)
{
  static char tok[256];
  tok[0] = '\0';
  skipws(f);
  char* p = &tok[0];
  int ch;
  while ( (ch = fgetchar(f)) != EOF && !char_in(ch, WHITESPACE)
      && p-tok<sizeof(tok) )
  {
      *p++ = ch;
  }
  *p = '\0';

  return tok;
}

bool islabel(const char* token)
{
  size_t l = strlen(token);
  return l<1? false : token[l-1] == ':';
}

bool iscomment(const char* token)
{
  return token[0] == ';';
}

Op tok2op(const char* token)
{
  return from_s(token);
}

bool isliteral(const char* token)
{
  if ( islabel(token) )
    return false;
  return tok2op(token) == NOP_END;
}

int main(int argc, char** argv)
{
  if ( argc <= 1 ) {
    fprintf(stderr, "Error: Need name of source file\n");
    return 1;
  }

  const char* filename = argv[1];
  machine_t m;
  FILE *f = fopen(filename, "rt");

  while ( !feof(f) ) {
    skipws(f);
    const char* t = token(f);
    if ( t[0] == '\0' ) {
      m.load_halt();
    } else if ( iscomment(t) ) {
      // skip to end of line
      skipto(f, "\n");
    } else if ( isliteral(t) ) {
      // convert literal: 0x12 / '\n' / 0123 / 123 to number
      int32_t literal = atoi(t);
      m.load(literal);
    } else if ( islabel(t) ) {
      m.labels.push_back(label_t(t, m.ip));
    } else {
      Op op = tok2op(t);

      if ( op == NOP_END ) {
        fprintf(stderr, "%s:%d:Unknown operation %s\n", filename, lineno, t);
        exit(1);
      } else if ( op == JMP ) {
        // implement jumping to labels
      }

      m.load(op);
    }
  }

  // just in case, add a halt instruction
  m.load_halt();
  fclose(f);

  // now save to same name with .sm suffix
  std::string str = filename;
  str += ".sm";
  f = fopen(str.c_str(), "wb");
  m.save_image(f);
  fprintf(stderr, "Compiled bytecode to file %s\n", str.c_str());
  fclose(f);

  return 0;
}
