/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "error.hpp"

void error(const char* s)
{
  fprintf(stderr, "\n%s\n", s);
  exit(1);
}
