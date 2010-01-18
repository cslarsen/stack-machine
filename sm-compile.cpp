#include <string>
#include <vector>
#include "sm-core.hpp"
#include "sm-util.hpp"
#include "sm-gencode.hpp"

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

int main(int argc, char** argv)
{
  try {
    if ( argc < 2 )
      error("Usage: sm-compile [ filename(s) ]");

    for ( int n=1; n<argc; ++n ) {
      fprintf(stderr, "Compiling %s", argv[1]);
      fflush(stderr);

      machine_t m;
      m = compile(fileptr(fopen(argv[1], "rt")), error);
      m.save_image( fileptr(
        fopen((basename(argv[1]) + ".sm").c_str(), "wb")));
    }

    return 0;
  }
  catch(const std::exception& e) {
    error(e.what());
  }
}
