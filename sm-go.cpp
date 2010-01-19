/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdio.h>
#include "sm-core.hpp"
#include "sm-util.hpp"
#include "fileptr.hpp"
#include "compiler.hpp"

void compile_and_run(const char* filename)
{
  parser p(fileptr(fopen(filename, "rt")));
  compiler c(p, error);
  c.get_program().run();
}

int main(int argc, char** argv)
{
  try {
    for ( int n=1; n<argc; ++n )
      compile_and_run(argv[n]);

    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
