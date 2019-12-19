#ifndef __FS_HPP_
#define __FS_HPP_
#include "uv.h"
#include "Common.hpp"
#include "Mutex.hpp"

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
      virtual ~Fs();

      virtual int open(const std::string& path, int flags, int mode);
      virtual int close(uv_file file);
      virtual int read(uv_file file, int64_t offset);
      virtual int write(uv_file file, int64_t offset);
      virtual int stat(const std::string&);

      void setCb(FsType );
      void invokeCb();
      void setState(FsState state);
      FsState getState() ;

      int enable_buf(unsigned int size);
      void release_buf();

      uv_fs_t* req();
      void clean_up();
    protected:
      uv_fs_cb _callback;
      
    private:
      
      /**
       * 表示类的状态
       **/ 
      FsState state = FsState::idle;

      /**
       * 用于存储和写入数据
       * is_use_buf标志是否使用了这一段数据
       **/ 
      uv_buf_t buf;
      bool is_use_buf = false;
      
      uv_fs_t _fs;
      EventLoop& _lp;
      FsType cb = nullptr;
      Mutex mx_for_new, mx_for_state;
  };
} // namespace xx


#endif