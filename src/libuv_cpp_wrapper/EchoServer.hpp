#ifndef _ECHO_SERVER_HPP
#define _ECHO_SERVER_HPP
#include "uvx.hpp"

namespace echo {
  class EchoServer {
    public:
      EchoServer();
      void run();
    private:
      uvx::Loop loop;
      uvx::Tcp tcpServer;
  };
}

#endif //_ECHO_SERVER_HPP