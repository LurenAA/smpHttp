#include <iostream>
#include <string>
#include "smpHttp.hpp"
#include "Mysql_con.hpp"
#include <nlohmann/json.hpp>
#include <codecvt>
#include <locale>
#include "jwt/jwt_all.h"
#include "stdlib.h"
#include <execinfo.h>
#include <exception>

using json = nlohmann::json;
using namespace std;
using namespace ::mysqlx;
Mysql_con mq("47.103.210.8", "root", "lab", "root");

void handle_json_lab(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  try{
    res->addHeader("Content-Type","application/json;charset=utf-8");
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
    res->addHeader("Access-Control-Max-Age", "3600");
    res->addHeader("Access-Control-Allow-Headers",
                  "Origin, X-Requested-With, Content-Type, Accept,token");
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
  } catch(...) {
    cout << "error in handle_json_lab" << endl;
  }
}

void handle_json_news(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  try {
    res->addHeader("Content-Type","application/json;charset=utf-8");
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
    res->addHeader("Access-Control-Max-Age", "3600");
    res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept,token");
    auto sqlres = mq.sql("Select * FROM labnews").execute();  
    json resj;
    resj["news"] = json::array();
    Row member_row;
    while(member_row = sqlres.fetchOne()){
      json tj = json::object();
      tj["id"] = static_cast<int>(member_row.get(0));
      tj["newTitle"] = smpHttp::Util::utf16Toutf8(member_row.get(1));
      tj["newContent"] = smpHttp::Util::utf16Toutf8(member_row.get(2));
      tj["newAuthor"] = smpHttp::Util::utf16Toutf8(member_row.get(3));
      tj["newDate"] = smpHttp::Util::utf16Toutf8(member_row.get(4));
      tj["newImage"] = smpHttp::Util::utf16Toutf8(member_row.get(5));
      resj["news"].push_back(tj);
    }
    std::string jso = resj.dump();
    res->addMessage(jso);
  } catch(...) {
    cout << "error in handle_json_lab" << endl;
  }
}

void handle_base(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  
}

void func(){
  int nptrs;
    #define SIZE 100
    void *buffer[100];
  
    nptrs = backtrace(buffer, SIZE);
  
    backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);

    #undef SIZE
}

int main(int argc, const char* argv[]) {
  set_terminate(func);
  try {
    smpHttp::HttpServer server;
    server.add_route("/json_lab", handle_json_lab);
    server.add_route("/json_news", handle_json_news);
    server.add_route("/", handle_base); //need to change
    server.add_static_path("/resources"); //add static route
    server.run();
  }catch(...) {
    int nptrs;
    #define SIZE 100
    void *buffer[100];
  
    nptrs = backtrace(buffer, SIZE);
  
    backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);

    #undef SIZE
  }
}

// terminate called after throwing an instance of 'std::length_error'
//   what():  basic_string::_M_create
