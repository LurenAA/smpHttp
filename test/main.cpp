#include <iostream>
#include "HttpServer.hpp"
using namespace std;

void routeHandleFunc1(std::shared_ptr<hpr::HttpResult> parseRes, uvx::Connection* cl){
  cout << 123 << endl;
}

int main() {
  smpHttp::HttpServer server;
  server.addRoute("/asd", routeHandleFunc1);
  server.run();
  
  return 0;
}