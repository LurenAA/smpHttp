#pragma once
#include <vector>
#include <regex>
#include <map>
#include <vector>
#include <set>

namespace smpHttp {
  class HttpResponse;
  class HttpRequest;
  using routeHandleFunc = void(*)(std::shared_ptr<HttpRequest>
    , std::shared_ptr<HttpResponse>);
  class HttpRouter {
  public: 
    routeHandleFunc add_route(const std::string&, routeHandleFunc);
    std::vector<routeHandleFunc> get_route(const std::string&);
    bool get_static_route(const std::string&);
    void add_static_route(std::string);
  private:
    std::map<std::string, routeHandleFunc> routes;
    std::set<std::string> static_routes;
    std::regex reg; 
  };
}