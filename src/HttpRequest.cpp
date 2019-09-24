#include <iostream>
#include "HttpRequest.hpp"

using namespace std;
using namespace smpHttp;

HttpRequest::HttpRequest(const HttpRequest& s) 
  : HttpResult(s), static_path(s.static_path)
{
}