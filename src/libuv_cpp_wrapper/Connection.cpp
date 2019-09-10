#include "Connection.hpp"
#include <iostream>
#include "Tcp.hpp"

using namespace std;
using namespace uvx;
void alloc_buf(uv_handle_t*, size_t, uv_buf_t*);

uv_read_cb Connection::readFunc = nullptr;
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
      delete [] buf;
    }
  );
}

void alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  uv_buf_init(new char[suggested_size], suggested_size);
}

void Connection::addReadFunc(uv_read_cb rd) {
  readFunc = rd;
}

Connection::Connection(uv_tcp_t* handle, Tcp* tcp) 
  : Handle(handle), tcp(tcp)
{
  req.data = this;
}

void Connection::write(const std::string& str) {
  if(!str.size()) {
    cerr << "warning: do not write a empty str" << endl;
    return ;
  } 
  reserve = str;
  buf.base = const_cast<char*>(reserve.c_str());
  buf.len = reserve.size();
  uv_write(&req, reinterpret_cast<uv_stream_t*>(handle.get()), &buf, 1, afterWrite);
}

void uvx::afterWrite(uv_write_t *req, int status) {
  if(status < 0) {
    Connection* con = static_cast<Connection*>(req->data);
    // con->tcp
  }
}