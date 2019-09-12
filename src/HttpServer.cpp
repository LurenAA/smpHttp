#include "HttpServer.hpp"
#include <iostream>

using namespace std;
using namespace uvx;
using namespace smpHttp;

void aConcClbk(uv_stream_t* server, uv_tcp_t* tcp);
void rdClbk(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

HttpServer::HttpServer() :
  loop(), tcpServer(loop)
{
  Tcp::connectionCallback = aConcClbk;
  Connection::readFunc = rdClbk;
}

void aConcClbk(uv_stream_t* server, uv_tcp_t* tcp) {
  Connection* cl = nullptr;
  if(tcp->data)
    cl = static_cast<Connection*>(tcp->data);
  cl->startRead();
}

void rdClbk(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  Connection* cl = nullptr;
  if(stream->data)
    cl = static_cast<Connection*>(stream->data);
  cl->write(buf->base, nread);
}

void HttpServer::run() {
  tcpServer.listen();
  loop.run();
}


