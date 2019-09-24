#include "HttpServer.hpp"
#include "HttpResult.hpp"
#include <cstring>
#include <iostream>
#include <iterator>
#include <functional>
#include "Packet.hpp"
#include <unistd.h>
#include <algorithm>
#include "Util.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

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
  shared_ptr<HttpRequest> parseRes(make_shared<HttpRequest>(parser.handleDatagram(buf->base, nread)));
  handleRoute(move(parseRes), cl);
};

void HttpServer::handleRoute(shared_ptr<HttpRequest> parseRes, Connection* cl) {
  //route
  string target = parseRes->getRequestTarget();
  if(route.route_static(target)) {
    //access to static resource
    // handleWrite(parseRes, cl, target);
    cout << parseRes.use_count() << endl;
    // deal_with_static()
  } else {
    void* func = route.route(target);
    if(!func) {
      handleWrite(parseRes, cl, "/http/404.html");
    } else {
      routeHandleFunc hFunc = reinterpret_cast<routeHandleFunc>(func);
      // hFunc(parseRes, cl);

      //do something
    }
  }
}

void HttpServer::handleWrite(shared_ptr<HttpResult> parseRes, Connection* cl, string staticPath) {
#define CHUNK_SIZE 50
  try {
    shared_ptr<IfstreamCon> fstrm = fstreamMap.at(staticPath);
    if(!fstrm->is_open()) {
      fstreamMap.erase(staticPath);
      cl->wfunc = nullptr;
      cl->write("0\r\n\r\n", 5);
      return ;
    }
    string s, shelper;
    int countSize = 0;
    while(countSize < CHUNK_SIZE && getline(fstrm->getFs(), shelper)) {
      Util::trim(shelper);
      s += shelper;
      countSize += shelper.size();
    }
    auto wfunc = bind(&HttpServer::handleWrite, this, parseRes, cl, staticPath);
    cl->wfunc = wfunc;
    // string res = Packet::chunk_data(s);

    if(countSize < CHUNK_SIZE) {
      fstrm->close();
    }
    // cl->write(res.c_str(), res.size());
  } catch(...) {
    //never use it before
    shared_ptr<IfstreamCon> newF(make_shared<IfstreamCon>());
    newF->open(Util::getRootPath() + staticPath);
    //to do : if path is error, ...

    if(!newF->is_open()) {
      cerr << "errno: newF is not open" << staticPath << endl;
      return ;
    }
    fstreamMap.insert({staticPath, newF});
    Packet chunkBegin;       
    //check the type
    chunkBegin.setContentType(staticPath);
    chunkBegin.addHeader("Transfer-Encoding", "chunked");
    string res = chunkBegin.get();
    auto wfunc = bind(&HttpServer::handleWrite, this, parseRes, cl, staticPath);
    cl->wfunc = wfunc;
    cl->write(res.c_str(), res.size());
  }
#undef CHUNK_SIZE
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

}