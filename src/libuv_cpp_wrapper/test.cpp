#include <iostream>
#include "TcpAccepter.hpp"
#include "FileLog.hpp"
using namespace std;
using namespace xx;

int main(void) {
  EventLoop lp(EventLoop::DEFAULT_LOOP);
  TcpAccepter acp(lp);
  acp.listen();
  lp.run();
  return 0; 
}