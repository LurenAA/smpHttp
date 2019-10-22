#ifndef __HANDLE_H__
#define __HANDLE_H__
#include <functional>
#include <memory>
#include "uv.h"

namespace uvx {
class Handle 
{
  public:
    Handle(uv_handle_t* handle);
    Handle(uv_tcp_t* handle);
    bool is_active();
    virtual ~Handle();
    virtual void close();
  protected:
    virtual void onClose() {}
    virtual bool isClosing();
    std::shared_ptr<uv_handle_t> handle;
};

}

#endif //Handle