/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * Synopsis:  Compile source to bydecode.
 *
 */

#include <stdexcept>
#include "instructions.hpp"
#include "sm-util.hpp"
#include "fileptr.hpp"
#include "compiler.hpp"

const char* file = "";
parser *p = NULL;

static void compile_error(const char* msg)
{
  fprintf(stderr, "%s:%d:%s\n", file, p->get_lineno(), msg);
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
      error("Usage: sm-compile [ filename(s) ]");

    for ( int n=1; n<argc; ++n ) {

      if ( !strcmp(argv[n], "-") )
        compile(stdin, "out.sm");
      else
        compile(fileptr(fopen(argv[n], "rt")), basename(argv[n]) + ".sm");
    }

    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
