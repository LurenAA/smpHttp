#ifndef _CONNECTION123_HPP_
#define _CONNECTION123_HPP_
#include "uv.h"
#include <memory>
#include "Handle.hpp"
#include <functional>
#include <string>

namespace uvx {
// using ReadFunc_t = std::function<void(uv_stream_t*, ssize_t, const uv_buf_t *)>;
// using Alloc_t = std::function<void (uv_handle_t*, size_t, uv_buf_t* buf)>;
class Tcp;
void afterWrite(uv_write_t *req, int status);
class Connection : public Handle, std::enable_shared_from_this<Connection>{
friend void afterWrite(uv_write_t *req, int status);
public:
  // using Handle::Handle;
  Connection(uv_tcp_t* handle, Tcp* tcp);
  void addReadFunc(uv_read_cb);
  void startRead();
  void write(const std::string& );
private:
  Tcp* tcp;
  uv_write_t req;
  uv_buf_t buf;
  std::string reserve; // reserve for buf 
  uv_tcp_t* getHandle();
  static uv_read_cb readFunc;
  static uv_alloc_cb allocFunc;
};
}

#endif //CONNECTION_HPP_