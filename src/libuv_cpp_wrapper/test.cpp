#include <iostream>
#include "TcpAccepter.hpp"
#include "FileLog.hpp"
#include "TcpConnection.hpp"
#include <string>
using namespace std;
using namespace xx;

int main(void) {
  EventLoop lp(EventLoop::DEFAULT_LOOP);
  TcpAccepter acp(lp);
  acp.listen();
  acp.setAfterConnectionCb([](std::shared_ptr<TcpConnection> tc) {
    tc->setInReadCb([](std::shared_ptr<TcpConnection> tc, ssize_t nread, const uv_buf_t *buf, bool isEof) {
      if(!isEof) {
        cout << buf->base << endl;
      } else {
        cout << "EOF" << endl;
      }
    });
    tc->read();
    tc->write("HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\n123");
  });
  lp.run();
  return 0; 
}