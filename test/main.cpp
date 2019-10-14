#include <iostream>
#include <string>
#include "smpHttp.hpp"
#include "Mysql_con.hpp"
#include <nlohmann/json.hpp>
#include <codecvt>
#include <locale>
using json = nlohmann::json;
using namespace std;
using namespace ::mysqlx;
Mysql_con mq("47.103.210.8", "root", "lab", "root");

void handleJson(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  auto sqlres = mq.sql("Select content FROM labimformation where type = 'labIntroduction'").execute();
  auto i = sqlres.fetchOne();
  mysqlx::string s = i.get(0);
  res->addHeader("Content-Type","application/json;charset=utf-8");
  json j;
  auto sqlres2 = mq.sql("Select content FROM labimformation where type = 'labDirection'").execute();
  auto i2 = sqlres2.fetchOne();
  mysqlx::string s2 = i2.get(0);
  std::string s2_8 = std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t >{}.to_bytes(s2.c_str());
  vector<std::string> vec2 = smpHttp::Util::split(s2_8, ',');
  auto sqlres3 = mq.sql("Select content FROM labimformation where type = 'labImage'").execute();
  auto i3 = sqlres3.fetchOne();
  vector<std::string> vec3 = smpHttp::Util::split(static_cast<std::string>(i3.get(0)), ',');
  j["labIntroduction"] = std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t >{}.to_bytes(s.c_str());
  j["labDirection"] = vec2;
  j["labImage"] = vec3;
  std::string jso = j.dump();
  res->addMessage(jso);
}

int main(int argc, const char* argv[]) {
  smpHttp::HttpServer server;
  server.add_route("/json", handleJson);
  server.add_static_path("/resources"); //add static route
  server.run();
}
