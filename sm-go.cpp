/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * This is a very simple stack machine.
 *
 * It's Turing complete, and you can do anything you want
 * with it, as long as you do it within 1 Mb of memory.
 *
 * Currently, the only program it accepts is hardcoded.
 * Program and memory size will be parameters later on.
 *
 * To stop your programs, just loop forever, as in embedded systems;
 * if you jump to the current line, this is understood as an
 * infinite loop and main() will exit.  The code for that is:
 *
 * load(PUSH); load(ip+sizeof(int32_t)); load(JMP);
 *
 */

#include <stdio.h>
#include "sm-core.hpp"
#include "sm-util.hpp"
#include "sm-gencode.hpp"

int main(int argc, char** argv)
{
  try {
    for ( int n=1; n<argc; ++n ) {
      machine_t m = compile(fileptr(fopen(argv[n], "rb")), error);
      m.run();
    }

    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
