/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * Synopsis:  Compile source to bydecode.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "version.hpp"
#include "instructions.hpp"
#include "fileptr.hpp"
#include "compiler.hpp"
#include "error.hpp"

const char* file = "";
parser *p = NULL;

// Return '<this part>.<ext>' of a filename
static std::string sbasename(const std::string& s)
{
  using namespace std;
  const string::size_type p = s.rfind('.');
  return p == string::npos ? s : s.substr(0, p);
}

static void compile_error(const char* msg)
{
  fprintf(stderr, "%s:%d:%s\n", file, p->get_lineno(), msg);
  exit(1);
}

void compile(FILE* f, const std::string& out)
{
  delete(p);
  p = new parser(f);
  compiler c(*p, compile_error);
  c.get_program().save_image( fileptr(fopen(out.c_str(), "wb")));
}

int main(int argc, char** argv)
{
  try {
    if ( argc < 2 )
      error("Usage: smc [ filename(s) | - ]\n" VERSION);

    for ( int n=1; n<argc; ++n ) {
      if ( !strcmp(argv[n], "-") ) {
        file = "<stdin>";
        compile(stdin, "out.sm");
      } else {
        file = argv[n];
        compile(fileptr(fopen(argv[n], "rt")),
                sbasename(argv[n]) + ".sm");
      }
    }

    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
