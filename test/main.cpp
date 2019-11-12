#include "route.hpp"

extern 
mysqlx::Client cli;

int main() {
  smpHttp::HttpServer server;
  server.add_route("^/json_lab$", handle_json_lab);
  server.add_route(R"(^/json_news(\?([^/\?=&]+=[^/\?=&]+&)*[^/\?=&]+=[^/\?=&]+)?)", handle_json_news);
  server.add_route("^/login$", handle_login);
  server.add_route("^/.*", handle_base); //need to change
  server.add_route("^/get_members", get_members); 
  server.add_static_path(R"(^/resources.*)"); //add static route
  server.add_route("^/member_change$", handle_member_change);
  server.add_route("^/assets_change$", change_assets);
  server.add_route("^/get_assets",get_assets);
  server.run();
  cli.close();
  return 0;
}