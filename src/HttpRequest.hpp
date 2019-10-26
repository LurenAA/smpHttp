#ifndef __HTTP_REQUREST_HPP_
#define __HTTP_REQUREST_HPP_
#include "HttpResult.hpp"
#include <string>
#include "IfstreamCon.hpp"
#include "Connection.hpp"

namespace smpHttp {
  class HttpServer;
  class HttpRequest : public hpr::HttpResult{
    friend class HttpServer;
    public:
      HttpRequest() = default;
      ~HttpRequest();
      HttpRequest(const HttpRequest& s);
      HttpRequest(hpr::HttpResult* s, uvx::Connection* c)
        : hpr::HttpResult(s),static_path(s->getRequestPath()), connection(c) {}
      const std::string& getStaticPath() {return static_path;}
      std::shared_ptr<IfstreamCon> fstream = nullptr;
      void close() {connection->close();}
    private:
      std::string static_path = "";
      uvx::Connection* connection;
  };
}
#endif //__HTTP_REQUREST_HPP_