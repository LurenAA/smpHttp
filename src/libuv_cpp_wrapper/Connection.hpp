#ifndef _CONNECTION123_HPP_
#define _CONNECTION123_HPP_
#include "uv.h"
#include <memory>
#include "Handle.hpp"
#include <functional>
#include <string>

namespace uvx {
using ReadFunc_t = std::function<void(uv_stream_t*, ssize_t, const uv_buf_t *)>;
// using Alloc_t = std::function<void (uv_handle_t*, size_t, uv_buf_t* buf)>;
using WriteFunc_t = std::function<void()>;
class Tcp;

class Connection : public Handle, public std::enable_shared_from_this<Connection>{
public:
  // using Handle::Handle;
  Connection(uv_tcp_t* handle, Tcp* tcp);
  void startRead();
  void write(const char* str, int len);
  void close();
  const uv_buf_t* getBuf();
  uv_write_t* getReq();
  uv_tcp_t* getHandle();
  
  ReadFunc_t readFunc;
  WriteFunc_t wfunc;
private:
  Tcp* tcp;
  uv_write_t req;
  uv_buf_t buf;
  std::string reserve; // reserve for buf 
  std::shared_ptr<Connection> sharedMe();
  void setReserve(const char *, size_t);
  static uv_alloc_cb allocFunc;
};
}

#endif //CONNECTION_HPP_