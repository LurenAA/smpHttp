#ifndef __HTTP_REQUREST_HPP_
#define __HTTP_REQUREST_HPP_
#include "HttpResult.hpp"
#include <string>
#include "IfstreamCon.hpp"
#include "TcpConnection.hpp"
#include <iostream>
#include "EventLoop.hpp"

namespace xx {
  class HttpServer;
  class HttpRequest : public xx::HttpResult{
    friend class HttpServer;
    public:
      HttpRequest() = default;
      HttpRequest(const hpr::HttpResult& s, std::shared_ptr<uvx::Connection> c,uvx::Loop& lp)
        : hpr::HttpResult(s), connection(c), loop(lp) {}
      std::shared_ptr<IfstreamCon> fstream = nullptr;
      void close() {connection->close();}
    private:
      std::shared_ptr<uvx::Connection> connection;
      uvx::Loop& loop;
  };
}
#endif //__HTTP_REQUREST_HPP_