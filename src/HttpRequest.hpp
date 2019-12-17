#ifndef __HTTP_REQUREST_HPP_
#define __HTTP_REQUREST_HPP_
#include "HttpResult.hpp"
#include <string>
#include "TcpConnection.hpp"
#include <iostream>
#include "EventLoop.hpp"
#include "RouteWq.hpp"


namespace xx {
  class HttpServer;
  class HttpRequest : std::enable_shared_from_this<HttpRequest>, public xx::HttpResult {
    friend class HttpServer;
    public:
      static
      std::shared_ptr<HttpRequest> newHttpRequest(const xx::HttpResult& s, std::shared_ptr<xx::TcpConnection> c,xx::HttpServer& svr);

      void close();
      virtual ~HttpRequest() {}
      RouteWq& getRoute() { return _route;}
      HttpServer& getServer () const;
    private:
      HttpRequest(const xx::HttpResult& s, std::shared_ptr<xx::TcpConnection> c,xx::HttpServer& lp);

      std::shared_ptr<TcpConnection> connection;
      HttpServer& svr;
      RouteWq _route;
  };
  
  
}
#endif //__HTTP_REQUREST_HPP_