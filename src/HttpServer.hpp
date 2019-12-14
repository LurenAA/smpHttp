#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_
#include "XXLibuv.hpp"
#include "HttpParser.hpp"
#include <fstream>
#include <map>
#include <functional>
#include "IfstreamCon.hpp"
#include <set>
#include "HttpRouter.hpp"

namespace xx {
  class HttpResponse;
  class HttpRequest;
  using routeHandleFunc = void(*)(std::shared_ptr<HttpRequest>
    , std::shared_ptr<HttpResponse>);
  class HttpServer{
    public:
      explicit HttpServer(AddressInfo a = AddressInfo{"0.0.0.0", 8080}, EventLoop& lp = EventLoop(DEFAULT_LOOP));
      void add_route(std::string s, routeHandleFunc);
      void add_static_path(std::string s);
      void run() { tcpServer.run();}
      void setPort(int aport) {tcpServer.setPort(aport);}

    private:
      uvx::TcpAccepter& tcpServer;
      xx::HttpParser parser;
      
      HttpRouter route;

      void deal_with_static(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>&);
      void deal_with_error(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>,const std::string&);

      void handleRoute(std::shared_ptr<HttpRequest> parseRes, std::shared_ptr<uvx::Connection> cl);
      void afterRead(std::shared_ptr<uvx::Connection> cl);
      std::shared_ptr<uvx::Connection> theConnectionCallback(uvx::Tcp* server, uv_tcp_t* client);
      void theAfterConnectionCallback(std::shared_ptr<uvx::Connection> c);
  };
}
#endif //__HTTPSERVER_H_ 