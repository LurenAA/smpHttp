#include <iostream>
#include "HttpRequest.hpp"

using namespace std;
using namespace smpHttp;

HttpRequest::HttpRequest(const HttpRequest& s) 
  : HttpResult(s), static_path(s.static_path), connection(s.connection)
{
}

HttpRequest::~HttpRequest(){
  // connection->close();
}

HttpRequest::HttpRequest(HttpRequest&& s) :
  HttpResult(s), static_path(s.static_path), connection(s.connection)
{
  s.connection = nullptr;
}
