#include "Connection.hpp"
#include <iostream>
#include "Tcp.hpp"
#include <cstring>

using namespace std;
using namespace uvx;
void alloc_buf(uv_handle_t*, size_t, uv_buf_t*);

uv_alloc_cb Connection::allocFunc = alloc_buf;


uv_tcp_t* Connection::getHandle() {
  return reinterpret_cast<uv_tcp_t*>(handle.get());
}

void Connection::startRead() {
  if(!Connection::readCallback) {
    cerr << "warning: need a readCallback when startRead" << endl;
    return ;
  }
  uv_read_start(reinterpret_cast<uv_stream_t*>(handle.get()),
    Connection::allocFunc, [] (uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
      cout << "log: " << __LINE__ <<" : " << __FILE__ << " read : " << nread << endl;
      Connection* con = static_cast<Connection*>(stream->data);
      con->onStartRead(stream, nread, buf);
      delete [] buf->base;
  });
}

void alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  *buf = uv_buf_init(new char[suggested_size], suggested_size);
}

Connection::Connection(uv_tcp_t* handle, Tcp* tcp) 
  : Handle(handle), tcp(tcp)
{
  req.data = this;
}

void Connection::write(const char* str, int len) {
  if(len <= 0) {
    cerr << "warning: do not write a empty str" << endl;
    return ;
  } 
  setReserve(str, len);
  buf.base = const_cast<char*>(reserve.c_str());
  buf.len = reserve.size();
  uv_write(&req, reinterpret_cast<uv_stream_t*>(handle.get()), &buf, 1, [](uv_write_t *req, int status){
    Connection* con = static_cast<Connection*>(req->data);
    if(status < 0) {
      cerr << "log: close a connection" << endl;
      con->close();
    } 
    con->onWrite();
  });
}

std::shared_ptr<Connection> Connection::sharedMe(){
  return shared_from_this();
}

void Connection::setReserve(const char * str, size_t len) {
  reserve.clear();
  reserve.assign(str, len);
}

const uv_buf_t* Connection::getBuf() {
  return &buf;
}

uv_write_t* Connection::getReq() {
  return &req;
}

void Connection::onClose() {
  tcp->removeConnection(sharedMe());
}

ReadCallback Connection::setReadCallback(ReadCallback f) {
  ReadCallback pf = readCallback;
  readCallback = f;
  return pf;
}

WriteCallback Connection::setWriteCallback(WriteCallback f) {
  WriteCallback pf = writeCallback;
  writeCallback = f;
  return pf;
}

void Connection::onRead(Connection* c) {
  if(readCallback)
    readCallback(c);
}

void Connection::onWrite() {
  if(writeCallback)
    writeCallback();
}