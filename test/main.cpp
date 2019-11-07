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
#include <unistd.h>

#define EXPIRE  30 * 60

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
  ClientOption::POOL_MAX_SIZE, 100,
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
    auto tb = mq.getSchema("lab").getTable("labmembers");
    auto sqlres_member_date = tb.select("id", "photo", "name","education","experience","sort").execute();
    Row member_row;
    while(member_row = sqlres_member_date.fetchOne()){
      json tj = json::object();
      tj["id"] = smpHttp::Util::utf16Toutf8(member_row.get(0));
      tj["photo"] = smpHttp::Util::utf16Toutf8(member_row.get(1));
      tj["name"] = smpHttp::Util::utf16Toutf8(member_row.get(2));
      tj["education"] = smpHttp::Util::utf16Toutf8(member_row.get(3));
      tj["experience"] = smpHttp::Util::utf16Toutf8(member_row.get(4));
   
      if(!member_row.get(5).isNull() && static_cast<int>(member_row.get(5))) 
        teachersData.push_back(tj);
      else 
        studentsData.push_back(tj);
    }
    j["membersData"]["teachersData"] = teachersData;
    j["membersData"]["studentsData"] = studentsData;
    std::string jso = j.dump();
    res->addMessage(jso);
  } catch(exception& e) {
    cout << "error: " << __LINE__ << " : " << __func__ << " : "<< e.what() << endl;
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
  res->addHeader("Access-Control-Allow-Origin", "*");
  res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
  res->addHeader("Access-Control-Max-Age", "3600");
  res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept, token");
  res->addHeader("Access-Control-Expose-Headers", "token");
  if(req->getHeader("token").size() && req->getMethod() == hpr::POST) {
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
      // res->setHttpStatus(smpHttp::HTTP_FORBIDDEN);
      res->addHeader("token", "11");
      cout << "error: " << tfe.what() << endl;
      res->end();
    }
    catch(InvalidClaimError& tfe){ //expired
      res->addHeader("token", "12");
      cout << "error: " << tfe.what() << endl;
      res->end();
    }
    catch (InvalidTokenError &tfe) {
      // res->setHttpStatus(smpHttp::HTTP_FORBIDDEN);
      json header, payload;
      std::tie(header, payload) = JWT::Decode(req->getHeader("token"));
      std::cout << "Payload: " << payload << std::endl;
      cout << "error: " << tfe.what() << endl;
      res->addHeader("token", "13");
      res->end();
    }
  } 
}

void handle_member_change(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  if(req->getMethod() != hpr::POST) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use post");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labmembers");
    json j = json::parse(req->getData());
    auto tryd = tb.select("name").where(std::string("id='") + 
      static_cast<std::string>(j["id"]) + "'").execute();
    bool has_one = tryd.count() != 0;
    uv_fs_t* req = new uv_fs_t();
    std::string save_path = "./resources/pic" + static_cast<std::string>(j["id"]);
    // uv_fs_open(uv_default_loop(), req, save_path.c_str(), )
    if(has_one) {
      auto upd = tb.update().set("tel", Value(static_cast<std::string>(j["tel"]))).
        set("address", Value(static_cast<std::string>(j["address"]))).
        set("education", Value(static_cast<std::string>(j["education"]))).
        set("name", Value(static_cast<std::string>(j["name"]))).
        set("email",  Value(static_cast<std::string>(j["email"]))).
        set("experience", Value(static_cast<std::string>(j["experience"]))).
        set("photo", Value(static_cast<std::string>(j["photo"]))).
        where(std::string("id='") + static_cast<std::string>(j["id"]) + "'").
        execute();
      json jres;
      jres["status"] = "update";
      res->addMessage(jres.dump());
    } else {
      auto ind = tb.insert("tel","address","education","name","email","experience","photo","id", "sort").
        values(static_cast<std::string>(j["tel"]),
        static_cast<std::string>(j["address"]),
        static_cast<std::string>(j["education"]),
        static_cast<std::string>(j["name"]),
        static_cast<std::string>(j["email"]),
        static_cast<std::string>(j["experience"]),
        static_cast<std::string>(j["photo"]),
        static_cast<std::string>(j["id"]), 0).
        execute(); 
      json jres;
      jres["status"] = "insert";
      res->addMessage(jres.dump());
    }
  } catch(exception& e) {
    cout << "error: handle_member_change : " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
    return ;
  } 
}

void handle_login(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{ 
  if(req->getMethod() != hpr::POST) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use post");
    res->end();
    return ;
  }
  try{
    json j = json::parse(req->getData());
    std::string username(j["username"]),
      password(j["password"]);
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labmembers");
    std::string condition = "username='"+username+"' AND "+"password='"+password + "'";
    auto sqlres = tb.select("name","sort").where(std::move(condition)).limit(1).execute();
    res->addHeader("Content-Type","application/json;charset=utf-8");
    json rj = json::object();
    if(sqlres.count() == 0) {
      rj["status"] = false;
      res->addMessage(rj.dump());
      return ;
    }
    res->addHeader("Content-Type","application/json;charset=utf-8");
    Row r = sqlres.fetchOne();
    rj["status"] = true;
    rj["name"] = smpHttp::Util::utf16Toutf8(r.get(0));
    rj["sort"] = (bool)r.get(1);
    rj["expire"] = time(nullptr) * 1000 + EXPIRE * 1000;
    time_t tim = time(nullptr);
    tim += EXPIRE;
    HS256Validator signer("secret");
    json payload = {{"exp", tim}, {"username", username}};
    std::string token = JWT::Encode(signer, payload);
    res->addHeader("token", token);
    res->addMessage(rj.dump());
  } catch(exception& e) {
    cout << "error: handle_login : " << e.what() << endl;
    // res->setHttpStatus(smpHttp::HTTP_FORBIDDEN);
    res->addMessage(e.what());
    res->end();
    return ;
  } 
}

void get_members(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) {
  if(req->getMethod() != hpr::GET) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use get");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  auto mq = cli.getSession();
  auto tb = mq.getSchema("lab").getTable("labmembers");
  json j = {};
  j["members"] = json::array();
  try {
    auto tb_size = tb.count();
    j["members_total_size"] = tb_size;
    size_t req_page_size = req->getQuery("page_size").size() ? atoi(req->getQuery("page_size").c_str()) : 0;
    size_t req_page_num =  req->getQuery("page_num").size() ? atoi(req->getQuery("page_num").c_str()) : 0; 
    size_t start_n;
    if(!req_page_size || !req_page_num) { 
      start_n = 0;
      req_page_size = tb_size;
    } else {
      start_n = req_page_size * (req_page_num - 1) > tb_size ? tb_size : req_page_size * (req_page_num - 1);
      if(start_n == tb_size) {
        res->addMessage("out of range");
        res->end();
        return ;
      }
    }
    auto ress = tb.select("tel","address","education","name","email","experience","photo","id").
      orderBy("id").
      limit(req_page_size*req_page_num == 0 ? tb_size : req_page_size*req_page_num).
      execute();
    vector<Row> rows = ress.fetchAll();
    for(auto i = rows.begin() + start_n; i < rows.end(); ++i) {
      json j_p = {};
      j_p["tel"] = smpHttp::Util::utf16Toutf8(i->get(0));
      j_p["address"] = smpHttp::Util::utf16Toutf8(i->get(1));
      j_p["education"] = smpHttp::Util::utf16Toutf8(i->get(2));
      j_p["name"] = smpHttp::Util::utf16Toutf8(i->get(3));
      j_p["email"] = smpHttp::Util::utf16Toutf8(i->get(4));
      j_p["experience"] = smpHttp::Util::utf16Toutf8(i->get(5));
      j_p["photo"] = smpHttp::Util::utf16Toutf8(i->get(6));
      j_p["id"] = smpHttp::Util::utf16Toutf8(i->get(7));
      j["members"].push_back(j_p);
    }
    res->addMessage(j.dump());
    return ;
  } catch(exception& e) {
    cout << "error: get_members : " << e.what() << endl;
    res->addMessage(e.what());
    res->end();
    return ;
  } 
}

void change_assets(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) {
  if(req->getMethod() != hpr::POST) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use post to change_assets");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labassets");
    json j = json::parse(req->getData());
    bool has_one = static_cast<std::string>(j["id"]).size() != 0;
    if(has_one) {
      auto upd = tb.update().set("item", Value(static_cast<std::string>(j["item"]))).
        set("remark", Value(static_cast<std::string>(j["remark"]))).
        set("marker", Value(static_cast<std::string>(j["marker"]))).
        set("date", Value(static_cast<std::string>(j["date"]))).
        set("money",  Value(static_cast<double>(j["money"]))).
        where(std::string("id=") + to_string(static_cast<int>(j["id"]))).
        execute();
      json jres;
      jres["status"] = "update";
      res->addMessage(jres.dump());
    } else {
      auto ind = tb.insert("item","remark","marker","date","money").
        values(static_cast<std::string>(j["item"]),
        static_cast<std::string>(j["remark"]),
        static_cast<std::string>(j["marker"]),
        static_cast<std::string>(j["date"]),
        static_cast<double>(j["money"])).
        execute(); 
      json jres;
      jres["status"] = "insert";
      res->addMessage(jres.dump());
    }
  } catch(exception& e) {
    cout << "error: handle_assets_change : " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
    return ;
  } 
}

void get_assets(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) {
  if(req->getMethod() != hpr::GET) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use get");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labassets");
    json j = {};
    auto ress = tb.select("id","item","remark","marker","date","money").
      orderBy("id").
      execute();  
    vector<Row> rows = ress.fetchAll();
    for(auto i = rows.begin(); i < rows.end(); ++i) {
      json j_p = {};
      j_p["id"] = static_cast<int>(i->get(0));
      j_p["item"] = smpHttp::Util::utf16Toutf8(i->get(1));
      j_p["remark"] = smpHttp::Util::utf16Toutf8(i->get(2));
      j_p["marker"] = smpHttp::Util::utf16Toutf8(i->get(3));
      j_p["date"] = smpHttp::Util::utf16Toutf8(i->get(4));
      j_p["money"] = static_cast<double>(i->get(5));
      j["assets"].push_back(j_p);
    }
    res->addMessage(j.dump());
    return ;
  }catch(exception& e) {
    cout << "error: handle_assets_get: " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
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
  server.add_route("^/json_lab$", handle_json_lab);
  server.add_route(R"(^/json_news(\?([^/\?=&]+=[^/\?=&]+&)*[^/\?=&]+=[^/\?=&]+)?)", handle_json_news);
  server.add_route("^/login$", handle_login);
  server.add_route("^/.*", handle_base); //need to change
  server.add_route("^/get_members", get_members); 
  server.add_static_path(R"(^/resources.*)"); //add static route
  server.add_route("^/member_change$", handle_member_change);
  server.add_route("^/assets_change$", change_assets);
  server.add_route("^/get_assets$",get_assets);
  server.run();
  cli.close();
}