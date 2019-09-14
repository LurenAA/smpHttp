#ifndef __HTTP_PARSER_HPP_
#define __HTTP_PARSER_HPP_
#include <memory>
#include "HttpResult.hpp"

namespace hpr {
  class HttpParser {
    public: 
      std::shared_ptr<HttpResult> handleDatagram(const std::string& datagram);
      std::shared_ptr<HttpResult> handleDatagram(const char* str, int len);
    private:
      void parseMethod(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        std::shared_ptr<HttpResult>& res);
      void parseRequestTarget(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        std::shared_ptr<HttpResult>& res);
      void parseHttpVersion(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        std::shared_ptr<HttpResult>& res);
      void parseHeaders(std::string::const_iterator& iter,
        std::string::const_iterator& iend, 
        std::shared_ptr<HttpResult>& res);
  };
}

#endif//__HTTP_PARSER_HPP_