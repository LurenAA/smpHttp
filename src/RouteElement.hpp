#ifndef __ROUTE_ELEMENT_HPP
#define __ROUTE_ELEMENT_HPP
#include <memory>
#include <regex>
#include <string>
#include "Common.hpp"

namespace xx {
  class RouteWq;
  class HttpResponse;
  class HttpRequest;
  struct RouteElement;
  using routeHandleFunc = void(*)(std::shared_ptr<HttpRequest>
    , std::shared_ptr<HttpResponse>, std::shared_ptr<RouteWq>);
  struct RouteElement {
    RouteElement(std::regex reg, uint16_t pri, std::string prefix,routeHandleFunc callback,
     bool is_static, xx::Method m = Method::ALL);
    bool operator<(const RouteElement& ele) const;
    std::regex reg;
    uint16_t priority;
    std::string prefix; //前缀
    routeHandleFunc callback; //回调
    bool is_static;
    Method method;
  };
}
#endif