/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <ctype.h>
#include "upper.hpp"

std::string upper(const std::string& s)
{
  std::string r(s);

  for ( int n=0, l=s.length(); n<l; ++n )
    r[n] = toupper(r[n]);

  return r;
}

