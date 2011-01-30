/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 * Synopsis:  Run compiled bytecode.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "version.hpp"
#include "instructions.hpp"
#include "machine.hpp"
#include "fileptr.hpp"

static void help()
{
  printf("smr -- stack-machine run\n");
  printf("%s\n\n", VERSION);

  printf("Opcodes:\n\n");

  Op op=NOP; 
  do {
    printf("0x%x = %s\n", op, to_s(op));
    op = static_cast<Op>(op+1);
  } while ( op != NOP_END );

  printf("\nTo halt program, jump to current position:\n\n");
  printf("0x0 PUSH 0x%x\n", (unsigned int)sizeof(int32_t));
  printf("0x%x JMP\n\n", (unsigned int)sizeof(int32_t));
  printf("Word size is %lu bytes\n", sizeof(int32_t));

  exit(0);
}

int main(int argc, char** argv)
{
  try {
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
  catch(const std::exception& e) {
    fprintf(stderr, "%s\n", e.what());
    return 1;
  }
}
