#ifndef _CONNECTION123_HPP_
#define _CONNECTION123_HPP_
#include "uv.h"
#include <memory>
#include "Handle.hpp"
#include <functional>
#include <string>

namespace uvx {
class Connection;
using ReadCallback = std::function<void(Connection*)>;
using WriteCallback = std::function<void()>;
using StartRedFunc_t = std::function<void (uv_stream_t *, ssize_t , const uv_buf_t *)>;
class Tcp;

class Connection : public Handle, public std::enable_shared_from_this<Connection>{
public:

  Connection(uv_tcp_t* handle, Tcp* tcp);
  void startRead();
  void write(const char* str, int len);
  const uv_buf_t* getBuf();
  uv_write_t* getReq();
  uv_tcp_t* getHandle();
  ReadCallback setReadCallback(ReadCallback);
  WriteCallback setWriteCallback(WriteCallback);
  void onRead(Connection*);
  void onWrite();
  bool is_writable() {return uv_is_writable(reinterpret_cast<const uv_stream_t*>(handle.get()));}
  int get_send_buf_size() const ;
  void set_send_buf_size(int s);
  
private:
  Tcp* tcp;
  uv_write_t req;
  uv_buf_t buf;
  std::string reserve; // reserve for buf 
  std::shared_ptr<Connection> sharedMe();
  
  ReadCallback readCallback = nullptr;
  WriteCallback writeCallback = nullptr;
  virtual void onStartRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf){};
  void onClose() override;

  void setReserve(const char *, size_t);
  static void allocFunc(uv_handle_t*, size_t, uv_buf_t*);
};
}

#endif //CONNECTION_HPP_