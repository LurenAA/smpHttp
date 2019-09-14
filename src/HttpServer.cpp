#include "HttpServer.hpp"
#include "HttpResult.hpp"
#include <cstring>
#include <iostream>

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

void aConcClbk(uv_stream_t* server, uv_tcp_t* tcp);
void rdClbk(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

HttpParser HttpServer::parser;

HttpServer::HttpServer() :
  loop(), tcpServer(loop)
{
  Tcp::connectionCallback = aConcClbk;
  Connection::readFunc = rdClbk;
  tcpServer.data = this;
  loop.data = this;
}

void aConcClbk(uv_stream_t* server, uv_tcp_t* tcp) {
  Connection* cl = nullptr;
  if(tcp->data)
    cl = static_cast<Connection*>(tcp->data);
  Tcp* theTcp = cl->getTcp();
  cl->data = theTcp->data; //都当data绑定到HttpServer
  cl->startRead();
}

void rdClbk(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  Connection* cl = nullptr;
  if(stream->data)
    cl = static_cast<Connection*>(stream->data);
  shared_ptr<HttpResult> parseRes = HttpServer::parser.handleDatagram(buf->base, nread);
  cl->write(RESPONES, strlen(RESPONES));
}

void HttpServer::run() {
  tcpServer.listen();
  loop.run();
}


