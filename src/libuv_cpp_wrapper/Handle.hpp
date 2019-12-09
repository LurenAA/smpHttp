#ifndef __HANDLE_H__
#define __HANDLE_H__
#include <functional>
#include <memory>
#include "uv.h"
#include "Common.hpp"

#define HANDLE_METHODS(XX) \
XX(HANDLE, uv_handle_t)\
XX(DIR, uv_dir_t)\
XX(STREAM, uv_stream_t)\
XX(TCP, uv_tcp_t)\
XX(UDP, uv_udp_t)\
XX(PIPE, uv_pipe_t)\
XX(TTY, uv_tty_t)\
XX(POLL, uv_poll_t)\
XX(TIMER, uv_timer_t)\
XX(PREPARE, uv_prepare_t)\
XX(CHECK, uv_check_t)\
XX(IDLE, uv_idle_t)\
XX(ASYNC, uv_async_t)\
XX(PROCESS, uv_process_t)\
XX(FS_EVENT, uv_fs_event_t)\
XX(FS_POLL, uv_fs_poll_t)\
XX(SIGNAL, uv_signal_t)

namespace xx {
class Handle 
{
  public:
    enum HandleType {
      #define DEFINE_TYPE(name, type) \
        name,
      HANDLE_METHODS(DEFINE_TYPE)
      #undef  DEFINE_TYPE
    };

    Handle(HandleType);
    virtual ~Handle();

    uv_handle_t* handle();
    bool is_active() const;
    virtual bool is_closing() const;
    void setInCloseCb(InCloseCbType);

    virtual void invokeInCloseCb();
    virtual void close();
    virtual void close_cb();  //内部侧的回调

  protected:
    InCloseCbType in_close_cb;   //用户侧的回调
    HandleType _type;
    uv_handle_t* _handle;
};

}

#endif //Handle