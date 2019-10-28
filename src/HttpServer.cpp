#include "HttpServer.hpp"
#include <cstring>
#include <iostream>
#include <iterator>
#include <functional>
#include <unistd.h>
#include <algorithm>
#include "Util.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <string>
#include <HttpConnection.hpp>

#define DEBUG_FLAG 1

using namespace std;
using namespace uvx;
using namespace smpHttp;
using namespace hpr;
using namespace std::placeholders;

void HttpServer::afterRead(uvx::Connection* acl){
  HttpConnection* cl = dynamic_cast<HttpConnection*>(acl);
  #ifdef DEBUG_FLAG
    cout << "datagram: " << "\r\n" 
    << cl->getDatagram() << endl;
  #endif

  try {
    HttpResult* r = parser.handleDatagram(cl->getDatagram());
    if(!r) {
      cout << "error : " << __func__ << 
      " r is empty " << endl;
      cl->close();  
    }
    shared_ptr<HttpRequest> parseReq(new HttpRequest(*r ,cl));
    //to complete
    if(parseReq->getMethod() == hpr::OPTIONS) {
      shared_ptr<HttpResponse> res(make_shared<HttpResponse>(cl)); 
      res->addHeader("Access-Control-Allow-Origin", parseReq->getHeader("Origin"));
      res->addHeader("Access-Control-Allow-Methods","POST, GET, OPTIONS");
      res->addHeader("Access-Control-Max-Age","86400");
      res->addHeader("Connection","keep-alive");
      res->addHeader("Access-Control-Expose-Headers", "token");
      res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept, token");
      return ;
    }
    return handleRoute(parseReq, cl);
  } catch (std::exception& e){
    cl->close();
    cout<< "warn:" << __FILE__ << ": " 
      << __LINE__ <<" ：" <<  e.what() << endl;
  } 
};

void HttpServer::handleRoute(shared_ptr<HttpRequest> parseReq, Connection* cl) {
  //route
  string target = parseReq->getRequestPath();
  bool if_static_path = route.get_static_route(target);
  shared_ptr<HttpResponse> parseRes(new HttpResponse(cl), HttpResponseDeleter()); 
  /**
   * temporary handle ... to complete later
  **/ 
  if(target != "/") {
    vector<routeHandleFunc> funcs = route.get_route("/");
    funcs[0](parseReq, parseRes);
  }

  if(if_static_path) {
    //access to static resource
    parseReq->static_path =  target; 
    deal_with_static(parseReq,parseRes);
  } else {
    vector<routeHandleFunc> funcs = route.get_route(target);
    for(auto x : funcs) {
      x(parseReq,parseRes);
    }
    if(!funcs.size()) {
      parseRes->addMessage("no such route");
    }
  }
}

void HttpServer::add_route(std::string s, routeHandleFunc func){
  route.add_route(s, func);
}

void HttpServer::add_static_path(std::string s) {
  route.add_static_route(s);
}

void HttpServer::deal_with_static(std::shared_ptr<HttpRequest> req
    , std::shared_ptr<HttpResponse> res)
{
#define CHUNK_SIZE 50000
  if(!req->fstream){
    //never use it before
    shared_ptr<IfstreamCon> newF(make_shared<IfstreamCon>());
    newF->open(Util::getRootPath() + req->getStaticPath());

    if(!newF->is_open()) {
      //to do with error
      std::string s = "errno:" +  req->getStaticPath() 
      + " does not exist " +  __FILE__ + " " + to_string(__LINE__);
      cout << s << endl;
      res->addMessage(s);
      res->setAfterWrite(nullptr);
      return ;
    }
    // fstreamMap.insert({req->getStaticPath(), newF});
    req->fstream = newF;
    //check the type
    res->setContentType(req->getStaticPath());
  }


  if(req->fstream) {
    shared_ptr<IfstreamCon> fstrm = req->fstream;
    if(!fstrm->is_open()) {
      res->setAfterWrite(nullptr);
      cout << "log: close " << req->getStaticPath()  << endl ;
      //to do time wheel
      return ;
    }
    string s;
    fstrm->read(s, CHUNK_SIZE); //read from the fstream
    res->addMessage(s); //add message body
    bool if_set_next_func = false; //if need a next deal_with_static call
    IfstreamCon::size_type remain = fstrm->remainNum();
    if(remain == 0) {
      /**
       * for normal datagram
       **/ 
      if(res->getMode() == NORMAL && res->isFirst()) { 
        res->addHeader("Content-Length", to_string(s.size()));
      } 
      /**
       * for the last chunk 
       **/ 
      else if(res->getMode() == CHUNKED && !res->isFirst()){
        res->setLastChunked();
      } else {
        cout << "error:" << to_string(__LINE__) <<
        res->getMode() << " " << res->isFirst() << endl;
        return ;
      }
    } else {
      /**
       * for the first chunked one
       **/ 
      if(res->isFirst()) {
        res->setNotFirst();
        res->setMode(CHUNKED_FIRST);
      }
      /**
       * for a normal chunked one
       **/  
      else {
        res->setMode(CHUNKED);
      }
      if_set_next_func = true;
    }
    
    shared_ptr<HttpResponse> rres(new HttpResponse(*res));
    if(if_set_next_func) {
      if(res->getMode() == CHUNKED || res->getMode() == CHUNKED_FIRST) 
        rres->setMode(CHUNKED);
      auto wfunc = bind(&HttpServer::deal_with_static, this, req, rres);
      res->setAfterWrite(wfunc);    
    } else{
      /**
       * close the connection next loop
       **/ 
      fstrm->close();
      auto wfunc = bind(&HttpServer::deal_with_static, this, req, rres);
      res->setAfterWrite(wfunc);  
    }

    // cout << res->get() << endl;
  } 
#undef CHUNK_SIZE
}

void HttpServer::deal_with_error(std::shared_ptr<HttpRequest> req,
  std::shared_ptr<HttpResponse> res,const std::string& s)
{
  res->addMessage(s);
}

Connection* HttpServer::theConnectionCallback(Tcp* server, uv_tcp_t* client) {
  HttpConnection* cli = new HttpConnection(client, server);
  auto afterReadbind = bind(&HttpServer::afterRead, this, _1);
  cli->setReadCallback(afterReadbind);
  return cli;
}

void HttpServer::theAfterConnectionCallback(Connection* c){
  c->startRead();
}

HttpServer::HttpServer(std::string ip, int port) :
  tcpServer(ip, port)
{
  auto afb = bind(&HttpServer::theAfterConnectionCallback, this, _1);
  tcpServer.setAfterConnectionCallback(afb);
  auto cfb = bind(&HttpServer::theConnectionCallback, this, _1, _2);
  tcpServer.setConnectionCallback(cfb);
}