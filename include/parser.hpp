#include <stdio.h>

class parser
{
  FILE* f;
  int lineno;
  int update_lineno(int c);
  int fgetchar();
  void move_back(int c);
  void skip_whitespace();

public:
  parser(FILE* f);
  int get_lineno() const;
  const char* next_token();
  void skip_line();
};
