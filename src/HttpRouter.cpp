#include <iostream>
#include "HttpRouter.hpp"
#include <algorithm>

using namespace std;
using namespace smpHttp;

routeHandleFunc HttpRouter::add_route(const std::string& s, routeHandleFunc func) {
  auto it = routes.find(s);
  if(it == routes.end()) {
    routes.insert({s, func});
    return nullptr;
  } else {
    routeHandleFunc func_pre = routes.at(s);
    routes.at(s) = func;
    return func_pre;
  }
}

std::vector<routeHandleFunc> HttpRouter::get_route(const std::string&s ) {
  std::vector<routeHandleFunc> res;
  for(auto x: routes) {
    reg.assign(x.first);
    bool isTrue = regex_match(s, reg);
    if(isTrue)
      res.push_back(x.second);
  }
  return res;
}

bool HttpRouter::get_static_route(const std::string& s) {
  auto fid = find_if(static_routes.cbegin(), static_routes.cend(), 
  [&](const std::string& is) {
    auto b = s.cbegin();
    for(auto x : is) {
      if(x != *b++) 
        return false;
    }
    return true;
  });
  if(fid == static_routes.end()) 
    return false;
  else return true;
}

void HttpRouter::add_static_route(std::string s ) {
  if(s.back() != '/')
    s += '/';
  if(s.front() != '/')
    s += '/' ;
  static_routes.insert(s);
}