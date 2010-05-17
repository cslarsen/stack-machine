#include <stdio.h>
#include <string>

class fileptr {
  FILE* f;
public:
  fileptr(FILE *file);
  ~fileptr();
  operator FILE*() const;
};

std::string upper(const std::string& s);
std::string lower(const std::string& s);
std::string format(const char* fmt, ...);
std::string basename(const std::string& s);
void error(const char* s);
