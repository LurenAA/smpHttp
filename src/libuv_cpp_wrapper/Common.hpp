#ifndef __COMMON_H_
#define __COMMON_H_
#include <functional>
#include "uv.h"
namespace xx {
class Handle;
class TcpConnection;
using InCloseCbType = std::function<void(Handle*)>;
using AfterConnectionType = std::function<void(std::shared_ptr<TcpConnection>)>;
using InReadCbType = std::function<void(std::shared_ptr<TcpConnection>, ssize_t nread, const uv_buf_t *buf, bool isEof)>;
using AfterWriteType = std::function<void(std::shared_ptr<TcpConnection>)>;
}
#endif