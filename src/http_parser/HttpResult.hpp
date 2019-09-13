#ifndef __HTTP_RESULT_HPP_
#define __HTTP_RESULT_HPP_
#include <string>

namespace hpr {
  enum Method {
    GET = 1,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE
  };

  class HttpResult {
    friend class HttpParser;
    public:

    private:
      void setMethod(const std::string& str);
      Method method;
  };
}
#endif //__HTTP_RESULT_HPP_