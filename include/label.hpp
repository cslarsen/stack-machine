/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdlib.h>
#include <string>

#ifndef INC_LABEL_HPP
#define INC_LABEL_HPP

struct label_t {
  std::string name;
  int32_t pos;

  label_t(const std::string& name_, int32_t position)
    : name(name_), pos(position)
  {
  }
};

#endif
