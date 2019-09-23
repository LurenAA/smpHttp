#ifndef __HTTP_REQUREST_HPP_
#define __HTTP_REQUREST_HPP_
#include "HttpResult.hpp"

namespace smpHttp {
  class HttpRequest : public hpr::HttpResult{
    public:
      HttpRequest(hpr::HttpResult* s): hpr::HttpResult(s) {}
  };
}
#endif //__HTTP_REQUREST_HPP_