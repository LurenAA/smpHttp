#include "Route.hpp"
#include <iostream>

using namespace std;
using namespace smpHttp;


void Route::insert(std::string s, routeHandleFunc f) {
  rax.insert(s, static_cast<void*>(f));
}

void Route::add_static(std::string s) {
  static_path_set.insert(s);
}