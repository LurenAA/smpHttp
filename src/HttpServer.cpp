#include "HttpServer.hpp"
#include "HttpResult.hpp"
#include <cstring>
#include <iostream>
#include <fstream>
#include <iterator>
#include <functional>

using namespace std;
using namespace uvx;
using namespace smpHttp;
using namespace hpr;
using namespace radix;

#define RESPONES \
  "HTTP/1.1 200 OK\r\n" \
  "Content-Type: text/plain\r\n" \
  "Content-Length: 12\r\n" \
  "\r\n" \
  "hello hhhhh\n"

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
  shared_ptr<HttpResult> parseRes = that->parser.handleDatagram(buf->base, nread);
  // cl->write(RESPONES, strlen(RESPONES));
  handleWrite(that, parseRes, cl);
};

void smpHttp::handleWrite(HttpServer* that, shared_ptr<HttpResult> parseRes, Connection* cl) {
  static ifstream fstrm("http/404.html");
  if(!fstrm.is_open()) {
    cerr << "error: open fstrm error";
    return ;
  }
  string str; 
  
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


