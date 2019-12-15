#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_
#include "XXLibuv.hpp"
#include "HttpParser.hpp"
#include <fstream>
#include <map>
#include <functional>
#include "RouteWq.hpp"
#include <set>

namespace xx {
  class HttpResponse;
  class HttpRequest;
  using routeHandleFunc = void(*)(std::shared_ptr<HttpRequest>
    , std::shared_ptr<HttpResponse>, std::shared_ptr<RouteWq>);
  class HttpAccepter : public TcpAccepter {
    using TcpAccepter::TcpAccepter;
    TcpConnection* tcp_connection_object() override;
  };
  class HttpServer{
    public:
      explicit HttpServer(AddressInfo a = AddressInfo{"0.0.0.0", 8080}, 
        EventLoop* lp = new EventLoop(EventLoop::DEFAULT_LOOP));
      ~HttpServer();

      void run();
      // void add_route(std::string s, routeHandleFunc);
      // void add_static_path(std::string s);


    private:
      EventLoop* _lp;
      HttpAccepter _tcpServer;
      RouteWq _route;
      
      void in_read_second(std::shared_ptr<TcpConnection> tc);
      void in_read(std::shared_ptr<TcpConnection> tc, ssize_t nread, const uv_buf_t *buf, bool isEof);
      void after_connection(std::shared_ptr<TcpConnection> tc);
      // void deal_with_static(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>&);
      // void deal_with_error(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>,const std::string&);

      // void handleRoute(std::shared_ptr<HttpRequest> parseRes, std::shared_ptr<uvx::Connection> cl);
      // void afterRead(std::shared_ptr<uvx::Connection> cl);
      // std::shared_ptr<uvx::Connection> theConnectionCallback(uvx::Tcp* server, uv_tcp_t* client);
      // void theAfterConnectionCallback(std::shared_ptr<uvx::Connection> c);
  };
}
#endif //__HTTPSERVER_H_ 