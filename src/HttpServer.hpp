#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_
#include "uvx.hpp"
#include "RadixTree.hpp"
#include "HttpParser.hpp"
#include <fstream>
#include <map>
#include <functional>
#include "IfstreamCon.hpp"
#include <set>

namespace smpHttp {
  class HttpServer;
  void handleRoute(HttpServer* that, std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl);
  void afterRead(HttpServer* that, uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
  void handleWrite(HttpServer* that, std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl, std::string staticPath);
  // using routeHandleFunc = std::function<>; 
  /**
   * can not use "std::function", because there is 
   * not cast from std::function<...>
   * to void* , I therefore use typedef instead
   **/ 
  typedef void (*routeHandleFunc)(std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl);
  class HttpServer {
    friend void handleRoute(HttpServer* that, std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl);
    friend void handleWrite(HttpServer* that, std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl, std::string staticPath);
    public:
      HttpServer();
      void addRoute(std::string s, routeHandleFunc);
      void setStaticPath(std::string s);
      void run();

      void afterConnect(uv_stream_t* server, uv_tcp_t* tcp);
      void afterRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    private:
      uvx::Loop loop;
      uvx::Tcp tcpServer;
      radix::RadixTree rax;
      hpr::HttpParser parser;
      std::map<std::string, std::shared_ptr<IfstreamCon>> fstreamMap; 
      std::set<std::string> staticPathSet;
  };
}
#endif //__HTTPSERVER_H_ 