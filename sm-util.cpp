/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdexcept>
#include "sm-util.hpp"

std::string upper(const std::string& s)
{
  std::string r;

  for ( const char* p = s.c_str(); *p; ++p )
    r += toupper(*p);

  return r;
}

std::string lower(const std::string& s)
{
  std::string r;

  for ( const char* p = s.c_str(); *p; ++p )
    r += tolower(*p);

  return r;
}

void error(const char* s)
{
  fprintf(stderr, "\n%s\n", s);
  exit(1);
}

std::string basename(const std::string& s)
{
  std::string::size_type p = s.rfind('.');
  return p == std::string::npos ? s : s.substr(0, p);
}
