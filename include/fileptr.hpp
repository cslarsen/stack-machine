/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdio.h>

#ifndef INC_FILEPTR_HPP
#define INC_FILEPTR_HPP

class fileptr {
  FILE* f;
  fileptr(const fileptr&); // deny
  fileptr& operator=(const fileptr&); // deny
public:
  fileptr(FILE *file);
  ~fileptr();
  operator FILE*() const;
};

#endif
