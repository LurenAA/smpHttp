#ifndef __HTTP_RESULT_HPP_
#define __HTTP_RESULT_HPP_
#include <string>
#include <cstring>
#include <memory>
#include <map>

#define MethodValue(XX, ME, str) \
  XX(GET, ME, str) \
  XX(HEAD, ME, str) \
  XX(POST, ME, str) \
  XX(PUT, ME, str) \
  XX(DELETE, ME, str) \
  XX(CONNECT, ME, str) \
  XX(OPTIONS, ME, str) \
  XX(TRACE, ME, str) \

#define CASE(V, ME, str) \
  if(!strcasecmp(#V, str.c_str())) \
    ME = V;  \

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
      Method getMethod() {return method;}
      const std::string getRequestTarget() {return requestTarget;}
      double getHttpVersion() {return httpVersion;}
      const std::map<std::string, std::string> getHeaders() {return headers;}
    private:
      void setMethod(const std::string& str);
      void setMethod(std::string::const_iterator& iter, std::string::const_iterator& iend);
      void setRequestTarget(std::string::const_iterator& iter, std::string::const_iterator& iend);
      void setHttpVersion(std::string::const_iterator& iter, std::string::const_iterator& iend);
      Method method;
      std::string requestTarget; 
      double httpVersion;
      std::map<std::string, std::string> headers;
  };
}
#endif //__HTTP_RESULT_HPP_