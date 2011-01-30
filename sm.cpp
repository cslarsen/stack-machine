/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * Synopsis:  Compile and run code on-the-fly.
 *
 */

#include <stdio.h>
#include <string.h>
#include "instructions.hpp"
#include "fileptr.hpp"
#include "compiler.hpp"
#include "error.hpp"
#include "upper.hpp"

void compile_and_run(FILE* f)
{
  parser p(f);
  compiler c(p, error);
  c.get_program().run();
}

void help()
{
  printf("Usage: sm [ file(s] ]\n");
  printf("Compiles and runs source files on the fly.\n\n");
  exit(1);
}

int main(int argc, char** argv)
{
  try {
    if ( argc == 1 ) // by default, read standard input
      compile_and_run(stdin);
  
    for ( int n=1; n<argc; ++n )
      if ( argv[n][0]=='-' ) {
        if ( argv[n][1] == '\0' )
          compile_and_run(stdin);
        else
          help();
      } else
        compile_and_run(fileptr(fopen(argv[n], "rt")));

    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
