#ifndef __HTTP_PARSER_HPP_
#define __HTTP_PARSER_HPP_
#include <memory>
#include "HttpResult.hpp"
#include "Mutex.hpp"

namespace xx {
  class HttpParserError : public std::runtime_error {
    public: 
      HttpParserError(const char* e) : std::runtime_error(e) {}
  };
  class HttpParser {
    public: 
      static
      HttpParser& getInstance();

      HttpParser& operator=(const HttpParser&) = delete;
      HttpParser(const HttpParser&) = delete;
      HttpResult handleDatagram(const std::string& datagram);
      HttpResult handleDatagram(const char* str, int len);
    private:
      static
      HttpParser* pr;
      HttpParser() = default;
      void parseMethod(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        HttpResult* res);
      void parseRequestTarget(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        HttpResult* res);
      void parseHttpVersion(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        HttpResult* res);
      void parseHeaders(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        HttpResult* res);
      void parseQueries(HttpResult* res);
      void parseRequestPath(HttpResult* res);
      void parseContent(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        HttpResult* res);
      void check(HttpResult* res);
      static
      Mutex mx;
  };
}

#endif//__HTTP_PARSER_HPP_