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
  if(!Connection::readFunc) {
    cerr << "warning: need a readFunc when startRead" << endl;
    return ;
  }
  uv_read_start(reinterpret_cast<uv_stream_t*>(handle.get()),
    Connection::allocFunc, [] (uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
      cout << "log: " << __LINE__ <<" : " << __FILE__ << " read : " << nread << endl;
      Connection* con = static_cast<Connection*>(stream->data);
      if(nread <= 0) {
        cout << "log: close a connection" << endl;
        delete [] buf->base;
        con->close();
        return ;
      }
      if(con->remain == INT_MIN) {
        try {
          char* l = strstr(buf->base, "Content-Length:");
          char* r = strchr(l, '\r');
          string len_s(l + strlen("Content-Length:"), r);
          con->remain = atoi(len_s.c_str());
          char *d = strstr(buf->base, "\r\n\r\n");
          if(d)
            con->remain += d - buf->base + 4;
        } catch(exception& e) {
          cout << "log: " << __LINE__ <<" : " << __FILE__ << " : " 
          << e.what() << endl;
          delete [] buf->base;
          con->close();
          return ;
        }
      }
      con->remain -= nread;
      con->reserve_for_read += buf->base;
      if(con->remain <= 0) 
        con->readFunc(con);
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
    Connection* con = static_cast<Connection*>(req->data);
    if(status < 0) {
      cerr << "log: close a connection" << endl;
      con->close();
    } 
    if(con->wfunc) {
      con->wfunc();
    }
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

void Connection::close_cb() {
  tcp->removeConnection(sharedMe());
}