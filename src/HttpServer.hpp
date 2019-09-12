#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_

#include "uvx.hpp"

namespace smpHttp {
  class HttpServer {
    public:
      HttpServer();
      void run();
    private:
      uvx::Loop loop;
      uvx::Tcp tcpServer;
  };
}
#endif //__HTTPSERVER_H_ 