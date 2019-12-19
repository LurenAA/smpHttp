// #include "route.hpp"

// extern 
// mysqlx::Client cli;
// extern 
// std::string host;
// extern 
// int port;

// int main(int argc, char * argv[]) {
//   int opt=0;
//   while((opt=getopt_long(argc,argv,"p:H:h",long_option,NULL))!=-1)
//   {
//     switch(opt)
//     {
//       case 0: break;
//       case 'H': 
//         host = optarg;
//         break;
//       case 'p':
//         port = atoi(optarg);
//         if(port == 0) 
//           _help = 1; 
//         break;                       
//       case 'h': 
//         _help = 1;
//         break;
//       case '?':
//         std::cout << "错误的参数" << std::endl;
//         _help = 1;
//         break;
//     }
//     if(_help == 1) 
//       break;
//   }
//   if(_help == 1) {
//     std::cout << HELP_INFO << std::endl;
//     return 1;
//   }
//   smpHttp::HttpServer server;
//   server.setPort(port);
//   server.add_route("^/json_lab$", handle_json_lab);
//   server.add_route(R"(^/json_news(\?([^/\?=&]+=[^/\?=&]+&)*[^/\?=&]+=[^/\?=&]+)?)", handle_json_news);
//   server.add_route("^/login$", handle_login);
//   server.add_route("^/.*", handle_base); //need to change
//   server.add_route("^/get_members", get_members); 
//   server.add_static_path(R"(^/resources.*)"); //add static route
//   server.add_route("^/member_change$", handle_member_change);
//   server.add_route("^/assets_change$", change_assets);
//   server.add_route("^/get_assets",get_assets);
//   server.add_route("^/get_articles",get_articles);
//   server.add_route("^/change_articles", change_articles);
//   server.add_route("^/get_assets_inf",get_assets_inf);
//   server.run();
//   cli.close();
//   return 0;
// }
#include "HttpServer.hpp"
#include "HttpResponse.hpp"
#include "RouteWq.hpp"
#include "HttpRequest.hpp"
#include "QueueWork.hpp"
#include "unistd.h"
#include "Fs.hpp"
#include "Util.hpp"
#include <getopt.h>
#include "FsWork.hpp"
#include <nlohmann/json.hpp>
#include <codecvt>
#include <locale>
#include "jwt/jwt_all.h"
#include "stdlib.h"
#include <execinfo.h>
#include <exception>
#include <ctime>
#include <unistd.h>
#include <algorithm>
#include "Base64Decoder.hpp"

//帮助信息
#define HELP_INFO \
"Usage: smpHttp [OPTION...]\n" \
"-p, --port=PORT_NUMBER\t\t指明端口号,必须和前端保持一致,默认值为8080\n"  \
"-h, --help            \t\t显示帮助" 

int _help = 0; //记录命令行参数状态
static 
const struct option long_option[]={
   {"port",optional_argument,NULL,'p'},
   {"help",no_argument, &_help, 1},
   {NULL,0,NULL,0}
};

using json = nlohmann::json;
using namespace std;
using namespace ::mysqlx;
using namespace xx;
mysqlx::Client cli(mysqlx::SessionOption::USER, "root",
                   mysqlx::SessionOption::PWD, "root",
                   mysqlx::SessionOption::HOST, "47.103.210.8",
                   mysqlx::SessionOption::PORT, 33060,
                   mysqlx::SessionOption::DB, "wx",
                   mysqlx::SessionOption::SSL_MODE, mysqlx::SSLMode::DISABLED,
                   mysqlx::ClientOption::POOLING, true,
                   mysqlx::ClientOption::POOL_MAX_SIZE, 100,
                   mysqlx::ClientOption::POOL_QUEUE_TIMEOUT, 1000,
                   mysqlx::ClientOption::POOL_MAX_IDLE_TIME, 500);

void post_user(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, RouteWq &wq) {
  res->addHeader("Content-Type", "application/json;charset=utf-8");
  res->addHeader("Access-Control-Allow-Origin", "*");
  res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
  res->addHeader("Access-Control-Max-Age", "3600");
  res->addHeader("Access-Control-Allow-Headers",
                 "Origin, X-Requested-With, Content-Type, Accept, token");
  shared_ptr<QueueWork> wk = QueueWork::newQueueWork(req, res, req->getServer());
  auto wk_func = [](std::shared_ptr<QueueWork> wk) {
    auto res = wk->getRes();
    auto req = wk->getReq();
    auto mq = cli.getSession();
    auto tb = mq.getSchema("wx").getTable("users");
    json j = json::parse(req->getData());
      auto ind = tb.insert("avatarUrl", "name").values(
        static_cast<std::string>(j["avatarUrl"]),
        static_cast<std::string>(j["name"])
      ).execute();
      json jres;
      jres["status"] = "insert";
      auto io = tb.select(" max(id)").execute();
      jres["id"] = static_cast<int>(io.fetchOne().get(0));
      res->addMessage(jres.dump());
  };
  wk->setWorkCb(wk_func);
  wk->work();
}
void post_articles(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, RouteWq &wq) {
  res->addHeader("Content-Type", "application/json;charset=utf-8");
  res->addHeader("Access-Control-Allow-Origin", "*");
  res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
  res->addHeader("Access-Control-Max-Age", "3600");
  res->addHeader("Access-Control-Allow-Headers",
                 "Origin, X-Requested-With, Content-Type, Accept, token");
  shared_ptr<QueueWork> wk = QueueWork::newQueueWork(req, res, req->getServer());
  auto wk_func = [](std::shared_ptr<QueueWork> wk) {
    auto res = wk->getRes();
    auto req = wk->getReq();
    auto mq = cli.getSession();
    auto tb = mq.getSchema("wx").getTable("articles");
    json j = json::parse(req->getData());
    if(j["type"] == "delete") {
      json jres;
      std::string range = "(";
      for (auto &el : j["postId"].items())
      {
        range += to_string(static_cast<int>(el.value())) + ",";
      }
      range.back() = ')';
      auto res_remove = tb.remove().where(std::string("postId in ") + range).execute();
      if (res_remove.getAffectedItemsCount() < 0)
      {
        throw runtime_error("delete nothing");
      }
      jres["status"] = "delete";
      res->addMessage(jres.dump());
    } 
    else if(j["type"] == "update") {
      for(auto x = j.begin(); x!=j.end();x++){
        if(x.key() != "type" && x.key() != "postId") {
          if(x.key() != "id")
            tb.update().set(x.key(), Value(std::string(x.value()))).where(std::string("postId = ") + to_string((int)j["postId"])).execute();
          else 
            tb.update().set("id", Value(int(x.value()))).execute();
        }
      }
      json jres;
      jres["status"] = "update";
      res->addMessage(jres.dump());
    } 
    else if(j["type"] == "insert") {
      auto ind = tb.insert("date", "title", "imgSrc", "avatar", "content", "reading", "collection","headImgSrc", "author","detail","musicUrl","musicTitle","id").values(
        static_cast<std::string>(j["date"]),
        static_cast<std::string>(j["title"]),
        static_cast<std::string>(j["imgSrc"]),
        static_cast<std::string>(j["avatar"]),
        static_cast<std::string>(j["content"]),
        static_cast<std::string>(to_string(rand())),
        static_cast<std::string>(to_string(rand())),
        static_cast<std::string>(j["headImgSrc"]),
        static_cast<std::string>(j["author"]),
        static_cast<std::string>(j["detail"]),
        static_cast<std::string>(j["musicUrl"]),
        static_cast<std::string>(j["musicTitle"]),
        static_cast<int>(j["id"])
      ).execute();
      json jres;
      jres["status"] = "insert";
      res->addMessage(jres.dump());
    }
  };
  wk->setWorkCb(wk_func);
  wk->work();
}


void get_articles(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, RouteWq &wq) {
  res->addHeader("Content-Type", "application/json;charset=utf-8");
  res->addHeader("Access-Control-Allow-Origin", "*");
  res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
  res->addHeader("Access-Control-Max-Age", "3600");
  res->addHeader("Access-Control-Allow-Headers",
                 "Origin, X-Requested-With, Content-Type, Accept, token");
  shared_ptr<QueueWork> wk = QueueWork::newQueueWork(req, res, req->getServer());
  auto wk_func = [](std::shared_ptr<QueueWork> wk) {
    auto res = wk->getRes();
    auto req = wk->getReq();
    auto mq = cli.getSession();
    auto tb = mq.getSchema("wx").getTable("articles");
    json j = {};
    j["articles"] = json::array();
    try {
      /**
       * 返回id对应的或者所有
       **/ 
      RowResult ress;
      if(req->getQuery("id").size()) {
        ress = tb.select("date", "title", "imgSrc", "avatar", "content", "reading", "collection","headImgSrc", "author","detail","postId","musicUrl","musicTitle","id").where("id=" + req->getQuery("id")).execute();
      } else{
        ress = tb.select("date", "title", "imgSrc", "avatar", "content", "reading", "collection","headImgSrc", "author","detail","postId","musicUrl","musicTitle","id").execute();
      }
      vector<Row> rows = ress.fetchAll();
      for (auto i = rows.begin(); i < rows.end(); ++i)
      {
        json j_p = {};
        j_p["date"] = xx::Util::utf16Toutf8(i->get(0));
        j_p["title"] =xx::Util::utf16Toutf8(i->get(1));
        j_p["imgSrc"] =xx::Util::utf16Toutf8(i->get(2));
        j_p["avatar"] =xx::Util::utf16Toutf8(i->get(3));
        j_p["content"] =xx::Util::utf16Toutf8(i->get(4));
        j_p["reading"] =xx::Util::utf16Toutf8(i->get(5));
        j_p["collection"] =xx::Util::utf16Toutf8(i->get(6));
        j_p["headImgSrc"] =xx::Util::utf16Toutf8(i->get(7));
        j_p["author"] =xx::Util::utf16Toutf8(i->get(8));
        j_p["detail"] =xx::Util::utf16Toutf8(i->get(9));
        j_p["postId"] = static_cast<int>(i->get(10));
        j_p["musicUrl"] =xx::Util::utf16Toutf8(i->get(11));
        j_p["musicTitle"] =xx::Util::utf16Toutf8(i->get(12));
        j_p["id"] = static_cast<int>(i->get(13));
        j["articles"].push_back(j_p);
      }
      res->addMessage(j.dump());
    }
    catch(exception &e) {
      cout << "error: get_articles : " << e.what() << endl;
      res->addMessage(e.what());
      res->end();
      return;
    }
  };
  wk->setWorkCb(wk_func);
  wk->work();
}

int main(int argc, char * argv[]) {
  int opt=0;
  int port = 8080;
  while((opt=getopt_long(argc,argv,"p:h",long_option,NULL))!=-1)
  {
    switch(opt)
    {
      case 0: break;
      case 'p':
        port = atoi(optarg);
        if(port == 0) 
          _help = 1; 
        break;                       
      case 'h': 
        _help = 1;
        break;
      case '?':
        std::cout << "错误的参数" << std::endl;
        _help = 1;
        break;
    }
    if(_help == 1) 
      break;
  }
  if(_help == 1) {
    std::cout << HELP_INFO << std::endl;
    return 1;
  }
  HttpServer server(port);
  server.add_route("/articles.*", get_articles, xx::GET);
  server.add_route("/articles.*", post_articles, xx::POST);
  server.add_route("/users",post_user, xx::POST);
  server.run();
  return 0;
}