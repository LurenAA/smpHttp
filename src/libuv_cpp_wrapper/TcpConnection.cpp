#include "TcpConnection.hpp"
#include <iostream>
#include "TcpAccepter.hpp"
#include <cstring>
#include "FileLog.hpp"
#include "Tools.hpp"
#include "FileLog.hpp"

using namespace std;
using namespace xx;


/**
 * 开始从流读取
 **/ 
void TcpConnection::read() {
  ::uv_read_start(reinterpret_cast<uv_stream_t*>(_handle),
    [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
      *buf = uv_buf_init(new char[suggested_size], suggested_size);
    }, 
    [] (uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
      TcpConnection* con = static_cast<TcpConnection*>(stream->data);
      FileLog& fl = FileLog::getInstance();
      ostringstream s_stream;
      if(nread < 0) {
        if(nread == UV_EOF) {
          s_stream.clear();
          s_stream << "Tcpconnection " << con->getIndex() << " reach end of file"; 
          fl.info(s_stream.str(), __func__, __FILE__, __LINE__);
          con->invokeInReadCb(nread, buf, true);
          con->close();
        }
        else {
          s_stream.clear();
          s_stream << "TcpConnection " << con->getIndex() << " read error";
          fl.info(s_stream.str(), __func__, __FILE__, __LINE__);
          con->close();
        }
      } else {
        s_stream.clear();
        s_stream << "Tcpconnection " << con->getIndex() << " read " << nread;
        fl.info(s_stream.str(), __func__, __FILE__, __LINE__);
        con->invokeInReadCb(nread, buf, false);
      }
      delete [] buf->base;
      }
    );
}

/**
 * 获取回调函数
 **/ 
void TcpConnection::invokeInReadCb(ssize_t nread, const uv_buf_t *buf, bool isEof) {
  if(in_read_cb) 
    in_read_cb(shared_from_this(), nread, buf, isEof);
}

/**
 * 设置回调函数
 **/ 
void TcpConnection::setInReadCb(InReadCbType f) {
  in_read_cb = f;
}

/**
 * 写
 **/ 
void TcpConnection::write(std::string str) {
  static const string::size_type max_buf_size = get_send_buf_size();
  string::size_type len = str.size();
  ReqEntity* req_entity = new ReqEntity(shared_from_this());
  if(len > max_buf_size) {
    req_entity->reserve = str.substr(0, max_buf_size);
    req_entity->rest_string = str.substr(max_buf_size);
  } else {
    req_entity->reserve = str;
  }
  req_entity->init();
  uv_write(&req_entity->req, reinterpret_cast<uv_stream_t*>(_handle), &req_entity->buf, 1, [](uv_write_t *req, int status){
    ReqEntity* req_entity = static_cast<ReqEntity*>(req->data);
    if(status < 0) {
      auto& fl = FileLog::getInstance();
      auto& tl = Tools::getInstance();
      fl.debug("uv_write -> status < 0 : " + tl.get_uv_strerror_t(status), __func__, __FILE__, __LINE__);
      req_entity->cl->close();
      return ;
    }
    if(req_entity->rest_string.size()) {
      req_entity->cl->write(req_entity->rest_string);
    } else {
      req_entity->cl->invokeAfterWriteCb();
    }
    delete req_entity;
  });
}

void TcpConnection::write(const char* str, std::string::size_type len) {
  if(len <= 0) {
    auto& fl = FileLog::getInstance();
    fl.error("write -> len <= 0",__func__, __FILE__, __LINE__);
    return ;
  } 
  write(std::string(str, len));
}

void TcpConnection::setAfterWriteCb(AfterWriteType f) {
  after_write_cb = f;
}

void TcpConnection::invokeAfterWriteCb() {
  if(after_write_cb) {
    after_write_cb(shared_from_this());
  }
}

/**
 * Gets or sets the size of the send buffer that the operating system uses for the socket.
 **/ 
int TcpConnection::get_send_buf_size() const {
  int size = 0,
    sta = uv_send_buffer_size(_handle, &size);
  if(sta < 0) {
    auto& fl = FileLog::getInstance();
    auto& tl = Tools::getInstance();
    fl.error("Connection::send_buf_size() -> uv_send_buffer_size : " + 
      tl.get_uv_strerror_t(sta), __func__, __FILE__, __LINE__);
    return -1;
  }
  return size;
}

int TcpConnection::set_send_buf_size(int size) {
  auto& fl = FileLog::getInstance();
  auto& tl = Tools::getInstance();
  if(size < 0) {    
    fl.error("error: Connection::set_buf_size() size < 0", __func__, __FILE__, __LINE__);
    return -1;
  }
  size /= 2;
  size += 1;
  int sta = uv_send_buffer_size(_handle, &size);
  if(sta < 0) {
    fl.error("Connection::set_buf_size() -> uv_send_buffer_size : " + tl.get_uv_strerror_t(sta)
      , __func__, __FILE__, __LINE__);
    return -1;
  }
  return size * 2;
}

/**
 * 初始化连接
 **/ 
TcpConnection::TcpConnection(TcpAccepter& tcp, EventLoop& lp) : 
  Handle(TCP), _tcp_accepter(tcp), _lp(lp)
{
  int flag = ::uv_tcp_init(lp.handle(), handle());
  if(flag < 0) {
    auto& tl = Tools::getInstance();
    auto& fl = FileLog::getInstance();
    fl.error("TcpConnection -> uv_tcp_init error : " + tl.get_uv_strerror_t(flag)
      , __func__, __FILE__, __LINE__);
    terminate();
  }
}

uv_tcp_t* TcpConnection::handle() {
  return reinterpret_cast<uv_tcp_t*>(_handle);
}

/**
 * 关闭连接的回调函数
 **/ 
void TcpConnection::close_cb() {
  if(index != -1)
    _tcp_accepter.remove_tcp_connection(index);
}

EventLoop& TcpConnection::getLp() {
  return _lp;
}

bool TcpConnection::is_writable() const {
  return ::uv_is_writable(reinterpret_cast<uv_stream_t*>(_handle));
}