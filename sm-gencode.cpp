#include "sm-gencode.hpp"

static int lineno = 1;
static const char WHITESPACE[] = "\r\n\t ";

int get_lineno()
{
  return lineno;
}

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

char to_ord(const char* s, void (*compile_error)(const char* message))
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

  compile_error("Unknown character literal");
}

bool islabel_ref(const char* s)
{
  return s[0] == '&';
}

int32_t to_literal(const char* s, void (*compile_error)(const char* message))
{
  if ( isnumber(s) )
    return atoi(s);

  if ( ischar(s) )
    return to_ord(s, compile_error);

  compile_error("Unknown literal");
}

bool ishalt(const char* s)
{
  return toupper(s) == "HALT";
}

machine_t compile(FILE* f, void (*compile_error)(const char* message))
{
  machine_t m;

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

          if ( toupper(t+1) == "HERE" )
            compile_error("Label HERE is reserved");

          forwards.push_back(label_t(t+1, m.pos()));
        }
      }
      else
        literal = to_literal(t, compile_error);

      m.load(literal);
    } else if ( islabel(t) ) {
      m.addlabel(t, m.pos());
    } else {
      Op op = tok2op(t);

      if ( op == NOP_END )
        compile_error("Unknown operation");

      m.load(op);
    }

  }

  // just in case, add a halt instruction
  m.load_halt();

  // now update all forward jumps, since
  // we now know all code labels.
  // If a label is not found this time,
  // we should raise an error
  for ( int n=0; n<forwards.size(); ++n ) {
    int32_t adr = m.get_label_address(forwards[n].name.c_str());

    if ( adr == -1 )
      compile_error("Code label not found");

    // update label jump to address
    m.set_mem(forwards[n].pos, adr);
  }
 
  return m;
}


