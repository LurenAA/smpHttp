#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_
#include "uvx.hpp"
#include "RadixTree.hpp"
#include "HttpParser.hpp"

namespace smpHttp {
  class HttpServer {
    public:
      HttpServer();
      void run();
      static hpr::HttpParser parser;
    private:
      uvx::Loop loop;
      uvx::Tcp tcpServer;
      radix::RadixTree rax;
  };
}
#endif //__HTTPSERVER_H_ 