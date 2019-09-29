#include <iostream>
#include "smpHttp.hpp"
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

void test1(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  res->addMessage("hello world");
}

int main() {
  smpHttp::HttpServer server;
  server.add_static_path("/http"); //add static route
  server.add_route("/hello", test1);
  server.run();
  
  return 0;
}
