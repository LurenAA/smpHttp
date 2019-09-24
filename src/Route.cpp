#include "Route.hpp"
#include <iostream>
#include "Util.hpp"

using namespace std;
using namespace smpHttp;


void Route::insert(std::string s, routeHandleFunc f) {
  rax.insert(s, reinterpret_cast<void*>(f));
}

void Route::add_static(std::string s) {
  if(s.back() != '/')
    s += '/';
  if(s.front() != '/')
    s = '/' + s;
  static_path_set.insert(s);
}

std::string Route::route_static(std::string s) {
  if(!static_path_set.size())
    return "";
  auto ie = s.find_first_of('?');
  if(ie != string::npos){
    s = s.substr(0, ie);
  }
  for(auto eve : static_path_set) {
    bool isFit = Util::starts_with(s, eve);
    if(isFit)
      return s;
  }
  return "";
}
