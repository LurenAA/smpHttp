#include <iostream>
#include <string>
#include "smpHttp.hpp"
#include "mysqlx/xdevapi.h"
using namespace std;
using namespace ::mysqlx;


void test1(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  res->addMessage("hello world");
}

int main(int argc, const char* argv[]) {
  smpHttp::HttpServer server;
  server.add_static_path("/http"); //add static route
  server.add_route("/hello", test1);
  server.run();

  Session sess("mysqlx://root:root@47.103.210.8");

  Schema sch= sess.getSchema("test");

}
