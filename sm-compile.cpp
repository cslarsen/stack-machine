#include <string>
#include <vector>
#include "sm-core.hpp"

static int lineno = 1;
static const char* filename = "";

static const char WHITESPACE[] = "\r\n\t ";

static int fgetchar(FILE* f)
{
  int n = fgetc(f);
  if ( n=='\n' ) ++lineno;
  return n;
}

std::string toupper(const char* s)
{
  std::string p;
  while ( *s ) p += toupper(*s++);
  return p;
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

  if ( ch == '\n' ) --lineno;
  ungetc(ch, f);
}

static void skipto(FILE* f, const char* chars)
{
  int ch;

  while ( (ch = fgetchar(f)) != EOF
    && !char_in(ch, chars) )
      ; // loop

  if ( ch == '\n' ) --lineno;
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

bool isnumber(const char* s)
{
  while ( *s )
    if ( !isdigit(*s++) )
      return false;
  return true;
}

bool ischar(const char* s)
{
  size_t l = strlen(s);

  if ( l==3 && s[0]=='\'' && s[2]=='\'' && s[1]!='\\' )
    return true;

  if ( l==4 && s[0]=='\'' && s[3]=='\'' && s[1]=='\\' 
            && char_in(s[2], "trn0") )
    return true;

  return false;
}

char to_ord(const char* s)
{
  size_t l = strlen(s);

  if ( l == 3 ) // 'n'
    return s[1];
 
  if ( l == 4 ) // '\n'
    switch ( s[2] ) {
    case 't': return '\t';
    case 'r': return '\r';
    case 'n': return '\n';
    case '0': return '\0';
    }

  fprintf(stderr, "%s:%d:Unknown character literal %s\n", filename, lineno, s);
  exit(1);
}

bool islabel_ref(const char* s)
{
  return s[0] == '&';
}

int32_t to_literal(const char* s)
{
  if ( isnumber(s) )
    return atoi(s);

  if ( ischar(s) )
    return to_ord(s);

  fprintf(stderr, "%s:%d:Unknown literal %s\n", filename, lineno, s);
  exit(1);
}

bool ishalt(const char* s)
{
  return toupper(s) == "HALT";
}

int main(int argc, char** argv)
{
  if ( argc <= 1 ) {
    fprintf(stderr, "Error: Need name of source file\n");
    return 1;
  }

  filename = argv[1];
  machine_t m;
  FILE *f = fopen(filename, "rt");

  if ( f == NULL ) {
    fprintf(stderr, "Could not open %s\n", filename);
    exit(1);
  }

  // store CODE POINTS that will later
  // be filled with LABEL ADDRESSES
  std::vector<label_t> forwards;

  while ( !feof(f) ) {
    skipws(f);
    const char* t = token(f);
    if ( t[0] == '\0' ) {
      m.load_halt();
    } else if ( iscomment(t) ) {
      // skip to end of line
      skipto(f, "\n");
    } else if ( ishalt(t) ) {
      // add halt function
      m.load_halt();
    } else if ( isliteral(t) ) {
      // convert literal: 0x12 / '\n' / 0123 / 123 to number
      int32_t literal;

      if ( islabel_ref(t) ) {
        literal = m.get_label_address(t+1);
        if ( literal == -1 ) {
          // label was not found,
          // store current address and update it later on

          if ( toupper(t+1) == "HERE" ) {
            fprintf(stderr, "%s:%d:Special label HERE is reserved\n", filename, lineno);
            exit(1);
          }

          forwards.push_back(label_t(t+1, m.ip));
        }
      }
      else
        literal = to_literal(t);

      m.load(literal);
    } else if ( islabel(t) ) {
      m.addlabel(t, m.ip);
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

  // now update all forward jumps, since
  // we now know all code labels.
  // If a label is not found this time,
  // we should raise an error
  for ( int n=0; n<forwards.size(); ++n ) {
    int32_t adr = m.get_label_address(forwards[n].name.c_str());

    if ( adr == -1 ) {
      fprintf(stderr, "%s:Code label '%s' not found\n", filename, forwards[n].name.c_str());
      exit(1);
    }

    // update label jump to address
    m.memory[forwards[n].pos] = adr;
  }
  
  // now save to same name with .sm suffix
  std::string str = filename;
  str += ".sm";
  f = fopen(str.c_str(), "wb");
  m.save_image(f);
  fprintf(stderr, "Compiled bytecode to file %s\n", str.c_str());
  fclose(f);

  return 0;
}
