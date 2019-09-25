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

using namespace std;
using namespace uvx;
using namespace smpHttp;
using namespace hpr;
using namespace radix;
using namespace std::placeholders;

#define RES \
  "HTTP/1.1 200 OK\r\n" \
  "\r\n"

void HttpServer::afterConnect(uv_stream_t* server, uv_tcp_t* tcp){
  Connection* cl = nullptr;
  if(tcp->data)
    cl = static_cast<Connection*>(tcp->data);
  auto afterReadbind = bind(&HttpServer::afterRead, this, _1, _2, _3);
  cl->readFunc = afterReadbind;
  cl->startRead();
};

void HttpServer::afterRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf){
  Connection* cl = nullptr;
  if(stream->data)
    cl = static_cast<Connection*>(stream->data);
  if(nread < 0) {
    cl->close();
    return ;
  }
  shared_ptr<HttpRequest> parseReq(make_shared<HttpRequest>(parser.handleDatagram(buf->base, nread)));
  return handleRoute(move(parseReq), cl);
};

void HttpServer::handleRoute(shared_ptr<HttpRequest> parseReq, Connection* cl) {
  //route
  string target = parseReq->getRequestTarget();
  string static_path = route.route_static(target);
  shared_ptr<HttpResponse> parseRes(make_shared<HttpResponse>(cl)); 
  if(static_path.size()) {
    //access to static resource
    // handleWrite(parseRes, cl, target);
    parseReq->static_path =  static_path; 
    deal_with_static(parseReq,parseRes);
  } else {
    void* func = route.route(target);
    if(!func) {
      deal_with_error(parseReq, parseRes, "error: no such route");
    } else {

      //do something
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
#define CHUNK_SIZE 500
START:
  try {
    shared_ptr<IfstreamCon> fstrm = fstreamMap.at(req->getStaticPath());
    if(!fstrm->is_open()) {
      fstreamMap.erase(req->getStaticPath());
      res->setAfterWrite(nullptr);
      //to do time wheel
      return ;
    }
    string s, shelper;
    int countSize = 0;
    while(countSize < CHUNK_SIZE && getline(fstrm->getFs(), shelper)) {
      Util::trim(shelper);
      s += shelper;
      countSize += shelper.size();
    }

    if(countSize <= CHUNK_SIZE ) {
      if(res->isFirst())
        res->addHeader("Content-Length", to_string(s.size()));
      fstrm->close();
      fstreamMap.erase(req->getStaticPath());
      res->setAfterWrite(nullptr);
    } else if(res->isFirst() && countSize > CHUNK_SIZE){
      res->setMode(CHUNKED_FIRST);
    } 
      
    res->addMessage(s);

    if(res->isFirst())
      res->setNotFirst();
    
    if(res->getMode() != NORMAL) 
    {
      if(countSize <= CHUNK_SIZE) {
        res->setLastChunked();
      } else {
        shared_ptr<HttpResponse> rres(new HttpResponse(*res));
        if(res->getMode() == CHUNKED || res->getMode() == CHUNKED_FIRST) 
          rres->setMode(CHUNKED);
        auto wfunc = bind(&HttpServer::deal_with_static, this, 
          req, rres);
        res->setAfterWrite(wfunc);
      }
    }

    // cout << res->get() << endl;
  } catch(...) {
    //never use it before
    shared_ptr<IfstreamCon> newF(make_shared<IfstreamCon>());
    newF->open(Util::getRootPath() + req->getStaticPath());
    //to do : if path is error, ...

    if(!newF->is_open()) {
      //to do with error
      std::string s = "errno:" +  req->getStaticPath() 
      + " does not exist " +  __FILE__ + " " + to_string(__LINE__);
      cout << s << endl;
      res->addMessage(s);
      res->setAfterWrite(nullptr);
      return ;
    }
    fstreamMap.insert({req->getStaticPath(), newF});
    //check the type
    res->setContentType(req->getStaticPath());
    goto START;
  }
#undef CHUNK_SIZE
}

void HttpServer::deal_with_error(std::shared_ptr<HttpRequest> req,
  std::shared_ptr<HttpResponse> res,const std::string& s)
{
  res->addMessage(s);
}