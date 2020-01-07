#ifndef _CONNECTION123_HPP_
#define _CONNECTION123_HPP_
#include "uv.h"
#include <memory>
#include "Handle.hpp"
#include <functional>
#include <string>
#include <cstring>
#include "EventLoop.hpp"
#include "Common.hpp"

namespace xx {
class TcpAccepter;
class TcpConnection : public Handle, public std::enable_shared_from_this<TcpConnection>{
public:
  TcpConnection(TcpAccepter& , EventLoop&);
  uv_tcp_t* handle();
  virtual ~TcpConnection() {};

  void setIndex(int id) {index = id;}
  int getIndex() const {return index;}
  void close_cb() override;
  void read();
  void invokeInReadCb(ssize_t nread, const uv_buf_t *buf, bool isEof);
  void setInReadCb(InReadCbType f);
  void setAfterWriteCb(AfterWriteType f);
  AfterWriteType getAfterWriteCb() const {return after_write_cb;}
  void invokeAfterWriteCb();

  void write(const char* str, std::string::size_type len) ;
  void write(std::string str);

  bool is_writable() const;
  int get_send_buf_size() const ;
  int set_send_buf_size(int s);
  EventLoop& getLp();

private:
  TcpAccepter& _tcp_accepter;
  EventLoop& _lp;
  int index = -1; //标记链接在map中的索引

  InReadCbType in_read_cb = nullptr;
  AfterWriteType after_write_cb = nullptr;
};

struct ReqEntity {
  uv_write_t req;
  std::string reserve = ""; //buf.base的指向
  uv_buf_t buf;
  std::shared_ptr<TcpConnection> cl;
  std::string rest_string = "";
  explicit ReqEntity(std::shared_ptr<TcpConnection> cl) 
   :cl(cl)
  {}
  void init() {
    ::memset(&req, 0, sizeof(req));
    buf.base = const_cast<char*>(reserve.c_str());
    buf.len = reserve.size();
    req.data = this;
  }
  void setReserve(const char * str, size_t len) {
    reserve.clear();
    reserve.assign(str, len);
  }
  void setReserve(const std::string &s) {
    setReserve(s.c_str(), s.size());
  }
};
}

#endif //CONNECTION_HPP_