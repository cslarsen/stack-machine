#include "sm-core.hpp"

int main(int argc, char** argv)
{
  machine_t m;
  m.load(PUSH); m.load(0);
  m.load(DUP); m.load(OUTNUM); 
  m.load(PUSH); m.load('\n'); m.load(OUT);

  m.load(PUSH); m.load(1);
  // 0 1

  int32_t label = m.ip;
  m.load(SWAP); // 1 0
  m.load(DUP);  // 1 0 0
  m.load(ROL3); // 0 0 1
  m.load(DUP);  // 0 0 1 1
  m.load(ROL3); // 0 1 1 0
  m.load(SWAP); // 0 1 0 1

  // add top two numbers
  m.load(ADD);  // 0 1 2
  m.load(DUP);  // 0 1 2 2

  // print result
  m.load(OUTNUM);  // 0 1 2
  m.load(PUSH); m.load('\n'); m.load(OUT);

  // goto label
  m.load(PUSH);
  m.load(label);
  m.load(JMP); // goto label

  m.load_halt();
  m.save_image(fileptr(fopen("fib.sm", "wb")));

  return 0;
}
