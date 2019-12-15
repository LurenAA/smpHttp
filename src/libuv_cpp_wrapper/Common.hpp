#ifndef __COMMON_H_PP
#define __COMMON_H_PP
#include <functional>
#include <string>
#include "uv.h"
#include <memory>

namespace xx {
class Handle;
class TcpConnection;
struct AddressInfo {
  std::string ip;
  int port;
};
enum {
  ERROR_BUF_SIZE = 1024
};
enum Method {
    GET = 1,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    ALL,
    NOTSET
  };
using InCloseCbType = std::function<void(Handle*)>;
using AfterConnectionType = std::function<void(std::shared_ptr<TcpConnection>)>;
using InReadCbType = std::function<void(std::shared_ptr<TcpConnection>, ssize_t nread, const uv_buf_t *buf, bool isEof)>;
using AfterWriteType = std::function<void(std::shared_ptr<TcpConnection>)>;
}
#endif