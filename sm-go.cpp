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
#include "sm-gencode.hpp"

int main(int argc, char** argv)
{
  try {
    for ( int n=1; n<argc; ++n )
      compile(fileptr(fopen(argv[n], "rb")), error).run();

    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
