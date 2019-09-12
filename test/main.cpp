#include <iostream>
#include "HttpServer.hpp"
using namespace std;

#define RESPONES \
   "HTTP/1.1 200 OK\r\n" \
    "Content-Type: text/plain\r\n" \
    "Content-Length: 12\r\n" \
    "\r\n" \
    "hello hhhhh\n"


int main() {
  smpHttp::HttpServer server;
  server.run();
  
  return 0;
}