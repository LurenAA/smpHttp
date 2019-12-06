#ifndef __COMMON_H_
#define __COMMON_H_
#include <functional>

namespace xx {
class Handle;
using CloseCbType = std::function<void(Handle*)>;
}
#endif