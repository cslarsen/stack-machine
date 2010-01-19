/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdexcept>
#include "sm-core.hpp"
#include "sm-util.hpp"
#include "fileptr.hpp"
#include "compiler.hpp"

const char* file = "";
parser *p = NULL;

static void compile_error(const char* msg)
{
  fprintf(stderr, "%s:%d:%s\n", file, p->get_lineno(), msg);
}

int main(int argc, char** argv)
{
  try {
    if ( argc < 2 )
      error("Usage: sm-compile [ filename(s) ]");

    for ( int n=1; n<argc; ++n ) {
      fprintf(stderr, "Compiling %s\n", argv[1]);

      fileptr f(fopen(argv[1], "rt"));
      delete(p);
      p = new parser(f);
      compiler c(*p, compile_error);

      std::string file = basename(argv[1]) + ".sm";
      c.get_program().save_image( fileptr(fopen(file.c_str(), "wb")));
    }

    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
