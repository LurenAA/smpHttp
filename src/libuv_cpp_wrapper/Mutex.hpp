#ifndef __XMUTEX_HPP_
#define __XMUTEX_HPP_
#include "pthread.h"
#include "Common.hpp"

namespace xx{
  enum {
    ERROR_BUF_SIZE = 1024
  };
  class Mutex {
    public:
      void lock();
      void unlock();
      bool try_lock();
      Mutex(const Mutex&) = delete;
      Mutex& operator=(const Mutex&) = delete;
    private:
      pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
  };

  class Pthread {
    using VoidVoidType = void(void);
    static void atfork(VoidVoidType prepare, VoidVoidType parent, VoidVoidType child);
  };
}

#endif