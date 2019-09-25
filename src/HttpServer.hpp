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
#include "Route.hpp"


namespace smpHttp {
  class HttpResponse;
  class HttpRequest;
  class HttpServer {
    public:
      HttpServer();
      void add_route(std::string s, routeHandleFunc);
      void add_static_path(std::string s);
      void run();

    private:
      uvx::Loop loop;
      uvx::Tcp tcpServer;
      hpr::HttpParser parser;
      std::map<std::string, std::shared_ptr<IfstreamCon>> fstreamMap; 
      Route route;

      void deal_with_static(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>);
      void deal_with_error(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>,const std::string&);

      void handleRoute(std::shared_ptr<HttpRequest> parseRes, uvx::Connection* cl);
      void afterConnect(uv_stream_t* server, uv_tcp_t* tcp);
      void afterRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
  };
}
#endif //__HTTPSERVER_H_ 