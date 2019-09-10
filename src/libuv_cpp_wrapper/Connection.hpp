#ifndef _CONNECTION123_HPP_
#define _CONNECTION123_HPP_
#include "uv.h"
#include <memory>
#include "Handle.hpp"

namespace uvx {
class Connection : public Handle{
public:
  using Handle::Handle;
};
}

#endif //CONNECTION_HPP_