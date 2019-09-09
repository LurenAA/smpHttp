#ifndef __HANDLE_H__
#define __HANDLE_H__
#include <functional>
#include <memory>
#include "uv.h"

namespace uvx {

typedef std::function<void ()> HandleCloseFunc;
class Handle 
{
  public:
    Handle(uv_handle_t* handle,HandleCloseFunc close_cb = nullptr);
    virtual void executeClose_cb();
    virtual ~Handle();
    virtual void setClose_cb(HandleCloseFunc close_cb);
  protected:
    virtual bool isClosing();
    virtual void close();
    std::shared_ptr<uv_handle_t> handle;
    HandleCloseFunc close_cb;
};

}

#endif //Handle