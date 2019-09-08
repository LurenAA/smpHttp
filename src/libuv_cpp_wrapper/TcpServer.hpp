#ifndef __TCP_SERVER_H_
#define __TCP_SERVER_H_
#include "TcpAcceptor.hpp"
#include <memory>

namespace net {
  class TcpServer {
    public:

    private:
      std::shared_ptr<TcpAcceptor> acceptor;
      
  };
}
#endif