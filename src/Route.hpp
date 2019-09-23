#ifndef __ROUTE_H__
#define __ROUTE_H__
#include "RadixTree.hpp"
#include <set>
#include <memory>
#include <functional>
#include <string>

namespace smpHttp {
  class HttpRequest;
  class HttpResponse;
  using routeHandleFunc = void(*)(std::shared_ptr<HttpRequest>
    , std::shared_ptr<HttpResponse>);
  /**
   * can not use "std::function", because there is 
   * not cast from std::function<...>
   * to void* , I therefore use typedef instead
   **/ 
  class Route{
    public:
      void insert(std::string s, routeHandleFunc);
      void add_static(std::string s);
      bool route_static(std::string s);
      void* route(const std::string &s) {return rax.route(s);}
    private:
      std::set<std::string> static_path_set;
      radix::RadixTree rax;
  };
}

#endif //__ROUTE_H__