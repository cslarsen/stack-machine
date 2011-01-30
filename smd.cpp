/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * Synopsis:  Disassemble bytecode.
 *
 */

#include <stdio.h>
#include "instructions.hpp"
#include "machine.hpp"
#include "fileptr.hpp"
#include "error.hpp"

static bool isprintable(int c)
{
  return (c>=32 && c<=127)
    || c=='\n'
    || c=='\r'
    || c=='\t';
}

static const char* to_s(char c)
{
  static char buf[2];
  buf[0] = c;
  buf[1] = '\0';

  switch ( c ) {
  default: return buf;
  case '\t': return "\\t";
  case '\n': return "\\n";
  case '\r': return "\\r";
  }
}

static void disassemble(machine_t &m)
{
  int32_t end = m.size();

  while ( m.pos() <= end ) {
    Op op = static_cast<Op>(m.cur());
    printf("0x%x %s", m.pos(), to_s(op));

    if ( (op==PUSH || op==PUSHIP) && m.pos()<=end ) {
        m.next();
        printf(" 0x%x", m.cur());

        if ( isprintable(m.cur()) )
          printf(" ('%s')", to_s(m.cur()));
    }

    printf("\n");
    m.next();
  }
}

int help()
{
  printf("Usage: smd [ file(s) }\n\n");
  printf("Disassembles compiled bytecode files.\n");
  exit(1);
}

int main(int argc, char** argv)
{
  try {
    for ( int n=1; n<argc; ++n ) {
      if ( argv[n][0] == '-' ) {
        if ( argv[n][1] != '\0' )
          help();
        continue;
      }

      machine_t m;
      m.load_image(fileptr(fopen(argv[n], "rb")));
      printf("; File %s --- %lu bytes\n", argv[n], m.size());
      disassemble(m);
    }
    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
