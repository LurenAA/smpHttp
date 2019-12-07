#include <iostream>
#include "TcpAccepter.hpp"
using namespace std;
using namespace xx;

int main(void) {
  EventLoop lp(EventLoop::DEFAULT_LOOP);
  TcpAccepter acp(lp);
  acp.listen();
  return 0; 
}