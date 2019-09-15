#include "HttpServer.hpp"
#include "HttpResult.hpp"
#include <cstring>
#include <iostream>
#include <iterator>
#include <functional>
#include "Url.hpp"

using namespace std;
using namespace uvx;
using namespace smpHttp;
using namespace hpr;
using namespace radix;

#define RES \
  "HTTP/1.1 200 OK\r\n" \
  "\r\n"

void trim(std::string& s) {
  if( !s.empty() )
  {
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1, s.size());
  }
}

void smpHttp::afterConnect(HttpServer* ts,uv_stream_t* server, uv_tcp_t* tcp){
  Connection* cl = nullptr;
  if(tcp->data)
    cl = static_cast<Connection*>(tcp->data);
  cl->startRead();
};

void smpHttp::afterRead(HttpServer* that, uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf){
  Connection* cl = nullptr;
  if(stream->data)
    cl = static_cast<Connection*>(stream->data);
  if(nread < 0) {
    cl->close();
    return ;
  }
  shared_ptr<HttpResult> parseRes = that->parser.handleDatagram(buf->base, nread);
  handleRoute(that, parseRes, cl);
};

void smpHttp::handleRoute(HttpServer* that, shared_ptr<HttpResult> parseRes, Connection* cl) {
  //route
  string target = parseRes->getRequestTarget();
  if(target == "/favicon.ico") {
    cl->wfunc = nullptr;    //temporary deal
    cl->write(RES, strlen(RES));
    return ;
  } else if(target.find("/http") != string::npos) {
    //access to static resource
    handleWrite(that, parseRes, cl, target);
  } else {
    void* func = that->rax.route(target);
    if(!func) {
      handleWrite(that, parseRes, cl, "http/404.html");
    } else {
      //do something
    }
  }
}

void smpHttp::handleWrite(HttpServer* that, shared_ptr<HttpResult> parseRes, Connection* cl, string staticPath) {
#define CHUNK_SIZE 50
  try {
    shared_ptr<IfstreamCon> fstrm = that->fstreamMap.at(staticPath);
    if(!fstrm->is_open()) {
      that->fstreamMap.erase(staticPath);
      cl->wfunc = nullptr;
      cl->write("0\r\n\r\n", 5);
      return ;
    }
    string s, shelper;
    int countSize = 0;
    while(countSize < CHUNK_SIZE && getline(fstrm->getFs(), shelper)) {
      trim(shelper);
      s += shelper;
      countSize += shelper.size();
    }
    auto wfunc = bind(handleWrite, that, parseRes, cl, staticPath);
    cl->wfunc = wfunc;
    string res = Url::chunk_data(s);
    if(countSize < CHUNK_SIZE) {
      fstrm->close();
    }
    cl->write(res.c_str(), res.size());
  } catch(...) {
    //never use it before
    shared_ptr<IfstreamCon> newF(make_shared<IfstreamCon>());
    newF->open(staticPath);
    if(!newF->is_open()) {
      cerr << "errno: newF is not open" << endl;
      return ;
    }
    that->fstreamMap.insert({staticPath, newF});
    Url chunkBegin;       
    chunkBegin.addHeader("Content-Type", "text/html");
    chunkBegin.addHeader("Transfer-Encoding", "chunked");
    string res = chunkBegin.get();
    auto wfunc = bind(handleWrite, that, parseRes, cl, staticPath);
    cl->wfunc = wfunc;
    cl->write(res.c_str(), res.size());
  }
#undef CHUNK_SIZE
}

HttpServer::HttpServer() :
  loop(), tcpServer(loop)
{
  auto afterConnectbind = bind(afterConnect, this, placeholders::_1, placeholders::_2);
  Tcp::connectionCallback = afterConnectbind;
  auto afterReadbind = bind(afterRead, this, placeholders::_1, placeholders::_2, placeholders::_3);
  Connection::readFunc = afterReadbind;
}

void HttpServer::run() {
  tcpServer.listen();
  loop.run();
}


