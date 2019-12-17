#ifndef __FS_HPP_
#define __FS_HPP_
#include "uv.h"
#include "Common.hpp"

namespace xx
{
  class EventLoop;
  class Fs {
    public:
      enum FsState {
        busy, 
        idle
      };
      Fs(EventLoop& _lp);

      int open(const std::string& path, int flags, int mode);
      void setCb(FsType );
      void invokeCb();

      uv_fs_t* req();
      void clean_up();
    private:
      FsState state = FsState::idle;
      uv_fs_t _fs;
      EventLoop& _lp;
      FsType cb = nullptr;
  };
} // namespace xx


#endif