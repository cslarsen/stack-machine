/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdexcept>
#include "fileptr.hpp"

fileptr::fileptr(FILE *file) : f(file)
{
  if ( f == NULL )
    throw std::runtime_error("Could not open file");
}

fileptr::~fileptr()
{
  fclose(f);
}

fileptr::operator FILE*() const
{
  return f;
}
