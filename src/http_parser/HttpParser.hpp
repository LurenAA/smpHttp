#ifndef __HTTP_PARSER_HPP_
#define __HTTP_PARSER_HPP_
#include <memory>
#include "HttpResult.hpp"

namespace hpr {
  class HttpParser {
    public: 
      HttpResult* handleDatagram(const std::string& datagram);
      HttpResult* handleDatagram(const char* str, int len);
    private:
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
  };
}

#endif//__HTTP_PARSER_HPP_