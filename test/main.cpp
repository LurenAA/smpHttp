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

void handle_json_lab(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  res->addHeader("Content-Type","application/json;charset=utf-8");
  res->addHeader("Access-Control-Allow-Origin", "*");
  res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
  res->addHeader("Access-Control-Max-Age", "3600");
  res->addHeader("Access-Control-Allow-Headers",
                "Origin, X-Requested-With, Content-Type, Accept,token");
  res->addHeader("Access-Control-Allow-Credentials", "true");
  auto sqlres = mq.sql("Select content FROM labimformation where type = 'labIntroduction'").execute();
  auto i = sqlres.fetchOne();
  std::string s_8 = smpHttp::Util::utf16Toutf8(i.get(0));
  json j;
  auto sqlres2 = mq.sql("Select content FROM labimformation where type = 'labDirection'").execute();
  auto i2 = sqlres2.fetchOne();
  std::string s2_8 = smpHttp::Util::utf16Toutf8(i2.get(0));
  vector<std::string> vec2 = smpHttp::Util::split(s2_8, ',');
  auto sqlres3 = mq.sql("Select content FROM labimformation where type = 'labImage'").execute();
  auto i3 = sqlres3.fetchOne();
  std::string s3_8 = smpHttp::Util::utf16Toutf8(i3.get(0));
  vector<std::string> vec3 = smpHttp::Util::split(s3_8, ',');
  j["labIntroduction"] = s_8;
  j["labDirection"] = vec2;
  j["labImage"] = vec3;
  j["membersData"] = json::object();
  json teachersData= json::array(),
    studentsData = json::array();
  auto sqlres_member_date = mq.sql("Select * FROM labmembers").execute();
  Row member_row;
  while(member_row = sqlres_member_date.fetchOne()){
    json tj = json::object();
    tj["id"] = static_cast<int>(member_row.get(0));
    tj["photo"] = smpHttp::Util::utf16Toutf8(member_row.get(1));
    tj["name"] = smpHttp::Util::utf16Toutf8(member_row.get(2));
    tj["education"] = smpHttp::Util::utf16Toutf8(member_row.get(3));
    tj["experience"] = smpHttp::Util::utf16Toutf8(member_row.get(4));
    if(static_cast<int>(member_row.get(5))) 
      teachersData.push_back(tj);
    else 
      studentsData.push_back(tj);
  }
  j["membersData"]["teachersData"] = teachersData;
  j["membersData"]["studentsData"] = studentsData;
  std::string jso = j.dump();
  res->addMessage(jso);
}

int main(int argc, const char* argv[]) {
  smpHttp::HttpServer server;
  server.add_route("/json_lab", handle_json_lab);
  server.add_static_path("/resources"); //add static route
  server.run();
}
