#include "sm-gencode.hpp"
#include "sm-util.hpp"

static int lineno = 1;

// "\t\n\r "
static const char WHITESPACE[256] = {
  0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// "trn0"
static const char TRN0[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0, 0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// "\n"
static const char ALL_BUT_LINEFEED[256] = {
  1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int get_lineno()
{
  return lineno;
}

static int fgetchar(FILE* f)
{
  int n = fgetc(f);

  if ( n=='\n' )
    ++lineno;

  return n;
}

static void skip(FILE* f, const char chars[256])
{
  int c;

  while ( (c = fgetchar(f)) != EOF
    && chars[c] )
      ; // loop

  if ( c == '\n' )
    --lineno;

  ungetc(c, f);
}

static const char* token(FILE* f)
{
  static char tok[256];
  char* p = &tok[0];
  int ch;

  tok[0] = '\0';
  skip(f, WHITESPACE);

  while ( (ch = fgetchar(f)) != EOF
      && !WHITESPACE[ch]
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
  return *token == ';';
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
            && TRN0[s[2]] )
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

  compile_error(format("Unknown character literal '%s'", s).c_str());
}

bool islabel_ref(const char* s)
{
  return *s == '&';
}

int32_t to_literal(const char* s, void (*compile_error)(const char* msg))
{
  if ( isnumber(s) )
    return atoi(s);

  if ( ischar(s) )
    return to_ord(s, compile_error);

  // unknown literal
  return -1;
}

bool ishalt(const char* s)
{
  return *s == '\0' || upper(s) == "HALT";
}

void check_label_name(const char* label, void (*compile_error)(const char*))
{
  if ( upper(label) == "HERE" )
    compile_error("Label HERE is reserved");
}

void compile_literal(
  machine_t& m,
  const char* t,
  std::vector<label_t>& forwards,
  void (*compile_error)(const char*))
{
  if ( islabel_ref(t) ) {
    int32_t label_adr = m.get_label_address(t+1);

    m.load(PUSH);

    // if label not found, mark it for update
    if ( label_adr == -1 ) {
      check_label_name(t+1, compile_error);
      forwards.push_back(label_t(t+1, m.pos()));
    }

    m.load(label_adr);

  } else {
    int32_t literal = to_literal(t, compile_error);

    if ( literal != -1 ) {
      // Perform implicit push (like Forth/Postscript)
      m.load(PUSH);
      m.load(literal);
    } else {
      // Treat unknown literal as function call

      // Push return address (4*wordsize -> continue after the last JMP here)
      m.load(PUSHIP);
      m.load(m.pos()+4*m.wordsize());

      // Push destination address, to be updated later
      m.load(PUSH);
      forwards.push_back(label_t(t, m.pos()));
      m.load(literal);

      // Perform the jump to the function
      m.load(JMP);

      // This is the point we'll return to with POPIP (m.pos()+4*m.wordsize())
    }
  }
}

void update_forward_jumps(
  machine_t& m,
  std::vector<label_t>& forwards,
  void (*compile_error)(const char*))
{
  int32_t address;

  for ( int n=0; n<forwards.size(); ++n ) {
    const char* label = forwards[n].name.c_str();
    int32_t address = m.get_label_address(label);

    if ( address == -1 )
      compile_error(format("Code label '%s' not found", forwards[n].name.c_str()).c_str());

    // update label jump to address
    m.set_mem(forwards[n].pos, address);
  }
}

machine_t compile(FILE* f, void (*compile_error)(const char* message))
{
  machine_t m;
  std::vector<label_t> forwards;
  lineno = 1;

  while ( !feof(f) ) {
    skip(f, WHITESPACE);

    const char* t = token(f);

         if ( ishalt(t) )    m.load_halt();
    else if ( iscomment(t) ) skip(f, ALL_BUT_LINEFEED);
    else if ( isliteral(t) ) compile_literal(m, t, forwards, compile_error);
    else if ( islabel(t) )   m.addlabel(t, m.pos());
    else {
      Op op = tok2op(t);

      if ( op == NOP_END )
        compile_error(format("Unknown operation on line %d", get_lineno()).c_str());

      m.load(op);
    }
  }

  // Add HALT-idiom, just in case
  m.load_halt();

  update_forward_jumps(m, forwards, compile_error);

  return m;
}
