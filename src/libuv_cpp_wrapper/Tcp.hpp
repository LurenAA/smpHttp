#ifndef __TCP_HPP_
#define __TCP_HPP_
#include "Handle.hpp"

namespace uvx {
  class Tcp : public Handle{
    public:
      Tcp(uv_handle_t* handle,HandleCloseFunc close_cb = nullptr);
  };
}
#endif