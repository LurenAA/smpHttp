#include "Connection.hpp"
#include <iostream>
#include "Tcp.hpp"
#include <cstring>

using namespace std;
using namespace uvx;

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

void Connection::allocFunc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
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
  if(len > INT_MAX ) {
    
  } else {
    setReserve(str, len);
    buf.base = const_cast<char*>(reserve.c_str());
    buf.len = reserve.size();
    // cout << "pre : " << get_send_buf_size() << " : " << buf.len << endl;
    set_send_buf_size(buf.len);
    // cout<< "after : " << get_send_buf_size() << endl;
    uv_write(&req, reinterpret_cast<uv_stream_t*>(handle.get()), &buf, 1, [](uv_write_t *req, int status){
      Connection* con = static_cast<Connection*>(req->data);
      if(status < 0) {
        cerr << "log: close a connection : " << uv_strerror(status) << endl;
        con->close();
      } 
      con->onWrite();
    });
  }
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

int Connection::get_send_buf_size() const {
  int size = 0,
    sta = uv_send_buffer_size(handle.get(),&size);
  if(sta < 0) {
    cout << "error : Connection::send_buf_size() -> uv_send_buffer_size : " <<
      uv_strerror(sta) << endl;
    return -1;
  }
  return size;
}

void Connection::set_send_buf_size(int size) {
  if(size < 0) {
    cout << "error: Connection::set_buf_size() size < 0" << 
    endl;
    return ;
  }
  size /= 2;
  size += 1;
  int sta = uv_send_buffer_size(handle.get(),&size);
  if(sta < 0) {
    cout << "error : Connection::set_buf_size() -> uv_send_buffer_size : " <<
      uv_strerror(sta) << endl;
    return ;
  }
}