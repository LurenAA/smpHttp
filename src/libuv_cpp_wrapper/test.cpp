#include <iostream>
#include "Loop.hpp"

using namespace std;
using namespace uvx;

int main(void) {
  Loop loop;
  loop.run();
  loop.close();
  return 0;
}