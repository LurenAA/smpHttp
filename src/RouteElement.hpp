#ifndef __ROUTE_ELEMENT_HPP
#define __ROUTE_ELEMENT_HPP
#include <memory>
#include <regex>
#include <string>
#include "CCommon.hpp"

namespace xx {
  class RouteWq;
  class HttpResponse;
  class HttpRequest;
  struct RouteElement;
  struct RouteElement {
    /**
     * 定义优先级
     **/ 
    enum {
      LOWER_DEFAULT_PRIORITY = 2,
      DEFAULT_PRIORITY = 3,
      HIGHER_DEFAULT_PRIORITY = 4,
      LOWER_FILE_PRIORITY = 18,
      FILE_PRIORITY = 19,
      HIGEHER_FILE_PRIORITY
    };
    RouteElement(std::regex reg,routeHandleFunc callback,
     bool is_static,std::string prefix = "",
     uint16_t pri = DEFAULT_PRIORITY ,xx::Method m = Method::ALL);
    bool operator<(const RouteElement& ele) const;
    std::regex reg;
    routeHandleFunc callback; //回调
    bool is_static;
    std::string prefix; //前缀
    uint16_t priority;
    Method method;
  };
}
#endif