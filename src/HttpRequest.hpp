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
  class HttpRequest : public xx::HttpResult{
    friend class HttpServer;
    public:
      HttpRequest(const xx::HttpResult& s, std::shared_ptr<xx::TcpConnection> c,xx::EventLoop& lp)
        : HttpResult(s), connection(c), _lp(lp), _route(this) {}
      void close() {connection->close();}
      virtual ~HttpRequest() {}
      
    private:
      std::shared_ptr<TcpConnection> connection;
      EventLoop& _lp;
      RouteWq _route;
  };
}
#endif //__HTTP_REQUREST_HPP_