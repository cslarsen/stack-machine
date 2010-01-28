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
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <stdarg.h>
#include <vector>
#include "sm-core.hpp"

static void help()
{
  Op op=NOP; 
  do {
    printf("0x%x = %s\n", op, to_s(op));
    op = static_cast<Op>(op+1);
  } while ( op != NOP_END );

  printf("\nTo halt program, jump to current position:\n\n");
  printf("0x0 PUSH 0x%x\n", sizeof(int32_t));
  printf("0x%x JMP\n\n", sizeof(int32_t));
  printf("Word size is %d bytes\n", sizeof(int32_t));

  exit(0);
}

int main(int argc, char** argv)
{
  bool found_file = false;

  for ( int n=1; n<argc; ++n ) {
    if ( argv[n][0] == '-' ) {
      help();
      continue;
    }
    
    found_file = true;
    machine_t m;
    m.load_image(fileptr(fopen(argv[n], "rb")));
    m.run();
  }

  if ( !found_file ) {
    machine_t m;
    m.load_image(stdin);
    m.run();
  }

  return 0;
}
