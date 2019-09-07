#include "Net.hpp"
#include <iostream>
using namespace std;

string routeFuncHad(RequestContext* q) {
  return "HTTP/1.1 200 OK\r\n" 
    "Content-Type: text/plain\r\n" 
    "Content-Length: 12\r\n" 
    "\r\n" 
    "hello hhhhh\n";
}

int main() {
  Net* net = new Net();
  net->change_port(8080);
  net->add_route("/", routeFuncHad);
  net->start_server();

  return 0;
}