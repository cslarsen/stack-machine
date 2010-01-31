#include <string>
#include <vector>
#include "sm-core.hpp"
#include "sm-util.hpp"
#include "sm-gencode.hpp"

void error(const char* s)
{
  fprintf(stderr, "%s\n", s);
  exit(1);
}

std::string chext(const std::string& s, const std::string& ext)
{
  std::string::size_type p;

  if ( (p = s.rfind(".")) != std::string::npos )
    return s.substr(0, p) + ext;
  else
    return s + ext;
}

int main(int argc, char** argv)
{
  if ( argc < 2 )
    error("Usage: sm-compile [ filename(s) ]");

  try {
    for ( int n=1; n<argc; ++n ) {
      fprintf(stderr, "Compiling %s", argv[1]);
      fflush(stderr);

      machine_t m = compile(fileptr(fopen(argv[1], "rt")), error);
      std::string out = chext(argv[1], ".sm");
      m.save_image(fileptr(fopen(out.c_str(), "wb")));
      
      fprintf(stderr, ", saved bytecode to %s\n", out.c_str());
      fflush(stderr);
    }
  }
  catch(const std::exception& e) {
    error(e.what());
  }

  return 0;
}
