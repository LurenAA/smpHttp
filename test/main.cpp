#include <iostream>
#include "HttpServer.hpp"
using namespace std;


int main() {
  smpHttp::HttpServer server;
  server.add_static_path("/http");
  server.run();
  
  return 0;
}