#include <iostream>
#include <string>
#include "smpHttp.hpp"
#include <nlohmann/json.hpp>
#include <codecvt>
#include <locale>
#include "jwt/jwt_all.h"
#include "stdlib.h"
#include <execinfo.h>
#include <exception>
#include <ctime>

#define EXPIRE  1 * 60

using json = nlohmann::json;
using namespace std;
using namespace ::mysqlx;
Client cli(SessionOption::USER, "root",
  SessionOption::PWD,  "root",
  SessionOption::HOST, "47.103.210.8",
  SessionOption::PORT, 33060,
  SessionOption::DB,   "lab",
  SessionOption::SSL_MODE, SSLMode::DISABLED,
  ClientOption::POOLING, true,
  ClientOption::POOL_MAX_SIZE, 10,
  ClientOption::POOL_QUEUE_TIMEOUT, 1000,
  ClientOption::POOL_MAX_IDLE_TIME, 500);

void handle_json_lab(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  try{
    Session mq = cli.getSession();
    res->addHeader("Content-Type","application/json;charset=utf-8");
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
    res->addHeader("Access-Control-Max-Age", "3600");
    res->addHeader("Access-Control-Allow-Headers",
                  "Origin, X-Requested-With, Content-Type, Accept, token");
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
  } catch(exception& e) {
    cout << "error: " << __LINE__ << " : " << __func__ << e.what() << endl;
  }
}

void handle_json_news(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  try {
    Session mq = cli.getSession();
    res->addHeader("Content-Type","application/json;charset=utf-8");
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
    res->addHeader("Access-Control-Max-Age", "3600");
    res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept, token");
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
  } catch(exception& e) {
    cout << "error: " << __LINE__ << " : " << __func__ << e.what() << endl;
  }
}

void handle_base(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  res->addHeader("Content-Type","application/json;charset=utf-8");
  res->addHeader("Access-Control-Allow-Origin", "*");
  res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
  res->addHeader("Access-Control-Max-Age", "3600");
  res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept, token");
  res->addHeader("Access-Control-Expose-Headers", "token");
  if(req->getHeader("token").size()) {
    ExpValidator exp1;
    try{
      HS256Validator signer("secret");
      ::json header, payload;
      std::tie(header, payload) = JWT::Decode(req->getHeader("token"), &signer, &exp1);
      payload["exp"] = time(nullptr) + EXPIRE;
      std::string token = JWT::Encode(signer, payload);
      res->addHeader("token", token);
    }
    catch (TokenFormatError &tfe) {
      res->addHeader("token", "error");
      cout << "error: " << tfe.what() << endl;
    }
    catch (InvalidTokenError &tfe) {
      json header, payload;
      std::tie(header, payload) = JWT::Decode(req->getHeader("token"));
      std::cout << "Payload: " << payload << std::endl;
      cout << "error: " << tfe.what() << endl;
      res->addHeader("token", "error");
    }
  } 
}

void handle_login(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  try{
    json j = json::parse(req->getData());
    std::string username(j["username"]),
      password(j["password"]);
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labmembers");
    std::string condition = "username='"+username+"' AND "+"password='"+password + "'";
    auto sqlres = tb.select("name","sort").where(std::move(condition)).limit(1).execute();
    if(sqlres.count() == 0) {
      res->setHttpStatus(smpHttp::HTTP_FORBIDDEN);
      res->addMessage("error password or username");
      return ;
    }
    res->addHeader("Content-Type","application/json;charset=utf-8");
    Row r = sqlres.fetchOne();
    json rj = json::object();
    rj["name"] = smpHttp::Util::utf16Toutf8(r.get(0));
    rj["sort"] = (bool)r.get(1);
    time_t tim = time(nullptr);
    tim += EXPIRE;
    HS256Validator signer("secret");
    json payload = {{"exp", tim}, {"username", username}};
    std::string token = JWT::Encode(signer, payload);
    // rj["token"] = token;
    res->addHeader("token", token);
    res->addMessage(rj.dump());
  } catch(exception& e) {
    cout << "error: handle_login : " << e.what() << endl;
    res->setHttpStatus(smpHttp::HTTP_FORBIDDEN);
    res->addMessage(e.what());
    return ;
  } 
}

void err_func(){
  int nptrs;
  #define SIZE 100
  void *buffer[100];
  nptrs = backtrace(buffer, SIZE);
  backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);
  #undef SIZE
}

int main(int argc, const char* argv[]) {
  set_terminate(err_func);
  smpHttp::HttpServer server;
  server.add_route("/json_lab", handle_json_lab);
  server.add_route("/json_news", handle_json_news);
  server.add_route("/login", handle_login);
  server.add_route("/", handle_base); //need to change
  server.add_static_path("/resources"); //add static route
  server.run();
  cli.close();
}