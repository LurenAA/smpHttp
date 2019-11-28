#ifndef __ROUTE_ELEMENT_HPP
#define __ROUTE_ELEMENT_HPP
#include <memory>
#include <regex>
#include <string>
enum Method {
  GET = 1,
  HEAD,
  POST,
  PUT,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE,
  ALL
};
namespace smpHttp {
  class RouteWq;
  class HttpResponse;
  class HttpRequest;
  struct RouteElement;
  using routeHandleFunc = void(*)(std::shared_ptr<HttpRequest>
    , std::shared_ptr<HttpResponse>,const RouteElement&, std::shared_ptr<RouteWq> rwq);
  union _CbkOrPfx{
    routeHandleFunc callback; //用于动态
    const char* prefix;   //用于静态路由
  };
  struct RouteElement {
    RouteElement(std::regex reg, uint16_t pri, _CbkOrPfx alx,
     bool is_static, Method m = Method::ALL);
    bool operator<(const RouteElement& ele) const;
    std::regex reg;
    uint16_t priority;
    _CbkOrPfx op;
    bool is_static;
    Method method;
    static void static_file_handle(std::shared_ptr<HttpRequest>
    , std::shared_ptr<HttpResponse>, const RouteElement&, std::shared_ptr<RouteWq>);
  };
}
#endif