#include "Connection.hpp"
#include <iostream>
#include "Tcp.hpp"

using namespace std;
using namespace uvx;
void alloc_buf(uv_handle_t*, size_t, uv_buf_t*);

ReadFunc_t Connection::readFunc = nullptr;
uv_alloc_cb Connection::allocFunc = alloc_buf;


uv_tcp_t* Connection::getHandle() {
  return reinterpret_cast<uv_tcp_t*>(handle.get());
}

void Connection::startRead() {
  if(!Connection::readFunc) {
    cerr << "warning: need a readFunc when startRead" << endl;
    return ;
  }
  uv_read_start(reinterpret_cast<uv_stream_t*>(handle.get()),
    Connection::allocFunc, [] (uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
      Connection::readFunc(stream, nread,  buf);
      delete [] buf->base;
    }
  );
}

void alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  *buf = uv_buf_init(new char[suggested_size], suggested_size);
}

Connection::Connection(uv_tcp_t* handle, Tcp* tcp) 
  : Handle(handle), tcp(tcp)
{
  req.data = this;
  wfunc = nullptr;
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
    afterWrite(req, status);
    Connection* con = static_cast<Connection*>(req->data);
    if(con->wfunc) {
      con->wfunc();
    }
  });
}

std::shared_ptr<Connection> Connection::sharedMe(){
  return shared_from_this();
}

void uvx::afterWrite(uv_write_t *req, int status) {
  if(status < 0) {
    Connection* con = static_cast<Connection*>(req->data);
    cerr << "log: close a connection" << endl;
    con->close();
  } 
}

void Connection::close() {
  this->tcp->removeConnection(sharedMe());
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