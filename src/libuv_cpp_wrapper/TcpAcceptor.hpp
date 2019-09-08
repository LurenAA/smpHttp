#ifndef __TCP_ACCEPTOR_H_
#define __TCP_ACCEPTOR_H_

#include "uv.h"
#include <functional>

#define DEFAULT_PORT 8080
#define DEFAULT_IP "127.0.0.1"

namespace net{
  typedef std::function<void(uv_loop_t*, uv_tcp_t*)> ConnectionCallback;
  class TcpAcceptor {
  public:
    TcpAcceptor(uv_loop_t* loop, const char* ip = DEFAULT_IP, int port = DEFAULT_PORT);
  };
}
#endif