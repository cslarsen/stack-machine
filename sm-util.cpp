#include <stdexcept>
#include "sm-util.hpp"

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
