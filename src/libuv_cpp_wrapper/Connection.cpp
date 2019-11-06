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
}

void Connection::write(std::string str) {
#define INT_MAX_ 20000 //INT_MAX
  string::size_type len = str.size();
  ReqEntity* req_entity = new ReqEntity();
  req_entity->cl = shared_from_this();
  if(len > INT_MAX_) {
    req_entity->reserve = str.substr(0, INT_MAX_);
    req_entity->rest_string = str.substr(INT_MAX_);
    int fl = get_send_buf_size();
    if(fl < INT_MAX_ && fl != -1)
      set_send_buf_size(INT_MAX_);
    else if(fl == -1){
      cerr << "error: close a connection " << endl;
      close();
      return ;
    }
  }
  else {
    req_entity->reserve = str;
    int fl = get_send_buf_size();
    if(fl < static_cast<int>(len) && fl > 0)
      set_send_buf_size(len);
    else if(fl == -1){
      cerr << "error: close a connection " << endl;
      close();
      return ;
    }
  }
  req_entity->init();
  uv_write(&req_entity->req, reinterpret_cast<uv_stream_t*>(handle.get()), &req_entity->buf, 1, [](uv_write_t *req, int status){
    ReqEntity* req_entity = static_cast<ReqEntity*>(req->data);
    if(status < 0) {
      cerr << "log: close a connection : " << uv_strerror(status) << endl;
      req_entity->cl->close();
      return ;
    }
    if(req_entity->rest_string.size()) {
      req_entity->cl->write(req_entity->rest_string);
    } else {
      req_entity->cl->onWrite();
    }
    delete req_entity;
  });
#undef INT_MAX_
}

void Connection::onClose() {
  tcp->removeConnection(shared_from_this());
}

void Connection::write(const char* str, std::string::size_type len) {
  if(len <= 0) {
    cerr << "warning: do not write a empty str" << endl;
    return ;
  } 
  write(std::string(str, len));
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

void Connection::onRead(std::shared_ptr<uvx::Connection> c) {
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