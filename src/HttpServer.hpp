#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_
#include "uvx.hpp"
#include "RadixTree.hpp"
#include "HttpParser.hpp"
#include <fstream>
#include <map>
#include "IfstreamCon.hpp"

namespace smpHttp {
  class HttpServer;
  void handleRoute(HttpServer* that, std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl);
  void afterConnect(HttpServer* ts,uv_stream_t* server, uv_tcp_t* tcp);
  void afterRead(HttpServer* that, uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
  void handleWrite(HttpServer* that, std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl, std::string staticPath);
  class HttpServer {
    friend void handleRoute(HttpServer* that, std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl);
    friend void afterRead(HttpServer* that, uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    friend void handleWrite(HttpServer* that, std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl, std::string staticPath);
    public:
      HttpServer();
      void run();
    private:
      uvx::Loop loop;
      uvx::Tcp tcpServer;
      radix::RadixTree rax;
      hpr::HttpParser parser;
      std::map<std::string, std::shared_ptr<IfstreamCon>> fstreamMap;
  };
}
#endif //__HTTPSERVER_H_ 