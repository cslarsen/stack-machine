#include <stdexcept>
#include "sm-core.hpp"
#include "sm-util.hpp"
#include "sm-gencode.hpp"

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
