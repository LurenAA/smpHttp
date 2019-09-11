#include <iostream>
#include "EchoServer.hpp"

using namespace std;
using namespace uvx;
using namespace echo;

int main(void) {
  EchoServer echoServer;
  echoServer.run();
  return 0;
}