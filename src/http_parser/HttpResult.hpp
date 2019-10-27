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
      HttpResult() = default;
      // HttpResult(const HttpResult&);
      HttpResult(HttpResult*);
      virtual ~HttpResult() {}
      Method getMethod() {return method;}
      const std::string getRequestTarget() {return requestTarget;}
      double getHttpVersion() {return httpVersion;}
      const std::string getRequestPath() {return requestPath;}
      const std::map<std::string, std::string>& getHeaders() {return headers;}
      const std::map<std::string, std::string>& getQueries() {return queries;}
      std::string getData() {return data;}
      std::string getHeader(const std::string&);
      std::string getQuery(const std::string&);
    private:
      void setMethod(const std::string& str);
      void setMethod(std::string::const_iterator& iter, std::string::const_iterator& iend);
      void setRequestTarget(std::string::const_iterator& iter, std::string::const_iterator& iend);
      void setHttpVersion(std::string::const_iterator& iter, std::string::const_iterator& iend);
      void setRequestPath(std::string&& s) {requestPath = std::move(s);}
      void setData(std::string&& s) {data = std::move(s);}
      Method method;
      std::string requestTarget; 
      std::string requestPath; 
      double httpVersion;
      std::map<std::string, std::string> headers;
      std::map<std::string, std::string> queries;
      std::string data;
  };
}
#endif //__HTTP_RESULT_HPP_