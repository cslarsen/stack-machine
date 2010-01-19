/*
 * Made in 2010 by Christian Stigen Larsen
 * http://csl.sublevel3.org
 *
 * Placed in the public domain by the author.
 *
 */

#include <stdio.h>
#include <string>

std::string upper(const std::string& s);
std::string lower(const std::string& s);
std::string format(const char* fmt, ...);
std::string basename(const std::string& s);
void error(const char* s);
