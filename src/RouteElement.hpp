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
    enum {
      DEFAULT_PRIORITY = 3,
      FILE_PRIORITY = 4
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