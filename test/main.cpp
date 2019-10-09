#include <iostream>
#include <string>
#include "smpHttp.hpp"
#include "Mysql_con.hpp"
using namespace std;
using namespace ::mysqlx;
// Mysql_con mq("192.168.204.1", "root", "root");

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
}
