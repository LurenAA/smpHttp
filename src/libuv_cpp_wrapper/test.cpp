#include <iostream>
#include "Loop.hpp"
#include "Tcp.hpp"

using namespace std;
using namespace uvx;

int main(void) {
  Loop loop;
  Tcp tcp(loop);
  tcp.listen();
  loop.run();
  loop.close();
  return 0;
}