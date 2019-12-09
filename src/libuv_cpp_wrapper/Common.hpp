#ifndef __COMMON_H_
#define __COMMON_H_
#include <functional>
#include "uv.h"
namespace xx {
class Handle;
class TcpConnection;
using InCloseCbType = std::function<void(Handle*)>;
using AfterConnectionType = std::function<void(std::shared_ptr<TcpConnection>)>;
}
#endif