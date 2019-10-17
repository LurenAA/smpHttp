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

#define DEBUG_FLAG 1

using namespace std;
using namespace uvx;
using namespace smpHttp;
using namespace hpr;
using namespace radix;
using namespace std::placeholders;

void HttpServer::afterConnect(uv_stream_t* server, uv_tcp_t* tcp){
  Connection* cl = nullptr;
  if(tcp->data)
    cl = static_cast<Connection*>(tcp->data);
  else 
    cout << "error: afterConnect no tcp->data" << endl;
  auto afterReadbind = bind(&HttpServer::afterRead, this, _1);
  cl->readFunc = afterReadbind;
  cl->startRead();
};

void HttpServer::afterRead( uvx::Connection* cl){
  // Connection* cl = nullptr;
  // if(stream->data)
  //   cl = static_cast<Connection*>(stream->data);
  // else 
  //   cout << "warn: error afterRead no stream->data" << endl;
  // if(nread < 0) {
  //   cout << "warn:" << __FILE__ << ": " 
  //     << __LINE__ << " :nread < 0" << endl;
  //   cl->close();
  //   return ;
  // } else if(nread == 0) {
  //   cout << "warn:" << __FILE__ << ": " 
  //     << __LINE__ << " :nread == 0" << endl;
  // } 
  #ifdef DEBUG_FLAG
    cout << "datagram: " << "\r\n" 
    << cl->reserve_for_read << endl;
  #endif

  try {
    HttpResult* r = parser.handleDatagram(cl->reserve_for_read);
    shared_ptr<HttpRequest> parseReq(make_shared<HttpRequest>(r ,cl));
    //to complete
    if(parseReq->getMethod() == hpr::OPTIONS) {
      shared_ptr<HttpResponse> res(make_shared<HttpResponse>(cl)); 
      res->addHeader("Access-Control-Allow-Origin", parseReq->getHeader("Origin"));
      res->addHeader("Access-Control-Allow-Methods","POST, GET, OPTIONS");
      res->addHeader("Access-Control-Max-Age","86400");
      res->addHeader("Connection","keep-alive");
      res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept,token");
      return ;
    }
    return handleRoute(move(parseReq), cl);
  } catch (std::exception& e){
    cl->close();
    cout<< "warn:" << __FILE__ << ": " 
      << __LINE__ <<" ：" <<  e.what() << endl;
  } 
};

void HttpServer::handleRoute(shared_ptr<HttpRequest> parseReq, Connection* cl) {
  //route
  string target = parseReq->getRequestPath();
  string static_path = route.route_static(target);
  shared_ptr<HttpResponse> parseRes(make_shared<HttpResponse>(cl)); 
  if(static_path.size()) {
    //access to static resource
    parseReq->static_path =  static_path; 
    deal_with_static(parseReq,parseRes);
  } else {
    /**
     * temporary handle ... to complete later
     **/ 
    if(target != "/") {
      void* base_func = route.route("/");
      if(base_func) {
        routeHandleFunc func_2 = reinterpret_cast<routeHandleFunc>(base_func);
        func_2(parseReq, parseRes);
      }
    } 

    void* func = route.route(target);
    if(!func) {
      deal_with_error(parseReq, parseRes, "error: no such route");
    } else {
      routeHandleFunc func_1 = reinterpret_cast<routeHandleFunc>(func);
      func_1(parseReq, parseRes);
    }
  }
}

HttpServer::HttpServer() :
  loop(), tcpServer(loop)
{
  auto afterConnectbind = bind(&HttpServer::afterConnect, this, _1, _2);
  tcpServer.connectionCallback = afterConnectbind;
}

void HttpServer::run() {
  tcpServer.listen();
  loop.run();
}

void HttpServer::add_route(std::string s, routeHandleFunc func){
  route.insert(s, func);
}

void HttpServer::add_static_path(std::string s) {
  route.add_static(s);
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