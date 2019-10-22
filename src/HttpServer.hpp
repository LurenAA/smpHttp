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
  class HttpServer{
    public:
      HttpServer(std::string ip = DEFAULT_IP, int port = DEFAULT_PORT);
      void add_route(std::string s, routeHandleFunc);
      void add_static_path(std::string s);
      void run() { tcpServer.run();}

    private:
      uvx::Tcp tcpServer;
      hpr::HttpParser parser;
      Route route;

      void deal_with_static(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>);
      void deal_with_error(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>,const std::string&);

      void handleRoute(std::shared_ptr<HttpRequest> parseRes, uvx::Connection* cl);
      void afterRead(uvx::Connection* cl);
      uvx::Connection* theConnectionCallback(uvx::Tcp* server, uv_tcp_t* client);
      void theAfterConnectionCallback(uvx::Connection* c);
  };
}
#endif //__HTTPSERVER_H_ 