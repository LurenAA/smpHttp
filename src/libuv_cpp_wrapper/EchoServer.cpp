#include "EchoServer.hpp"
#include <iostream>

using namespace std;
using namespace uvx;
using namespace echo;

void aConcClbk(uv_stream_t* server, uv_tcp_t* tcp);
void rdClbk(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

EchoServer::EchoServer() :
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
  // uv_write(cl->getReq(), reinterpret_cast<uv_stream_t*>(cl->getHandle()), 
  //   cl->getBuf(), 1, [](uv_write_t* req, int status) {
  //     if(status < 0) {
  //       cerr << "error: uv_write " << uv_strerror(status) << endl;
  //       exit(1);
  //     }
  //   });
}

void EchoServer::run() {
  tcpServer.listen();
  loop.run();
}


