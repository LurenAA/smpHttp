#ifndef __HTTP_REQUREST_HPP_
#define __HTTP_REQUREST_HPP_
#include "HttpResult.hpp"
#include <string>
#include "IfstreamCon.hpp"

namespace smpHttp {
  class HttpServer;
  class HttpRequest : public hpr::HttpResult{
    friend class HttpServer;
    public:
      HttpRequest() = default;
      HttpRequest(const HttpRequest& s);
      HttpRequest(hpr::HttpResult* s): hpr::HttpResult(s) {}
      const std::string& getStaticPath() {return static_path;}
      std::shared_ptr<IfstreamCon> fstream = nullptr;
    private:
      std::string static_path = "";
  };
}
#endif //__HTTP_REQUREST_HPP_