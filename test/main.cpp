#include <iostream>
#include "HttpServer.hpp"
using namespace std;

int main() {
  smpHttp::HttpServer server;
  server.run();
  
  return 0;
}