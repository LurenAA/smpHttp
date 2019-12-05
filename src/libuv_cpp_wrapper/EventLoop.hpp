#ifndef __LOOP_HPP_
#define __LOOP_HPP_
#include "uv.h"
#include <memory>
#include <atomic>

namespace xx{

class EventLoop{
public:
  enum LoopMode{
    NEW_LOOP,
    DEFAULT_LOOP
  };

  EventLoop(LoopMode m = DEFAULT_LOOP);
  ~EventLoop();
  EventLoop(const EventLoop&) = delete;
  EventLoop& operator=(const EventLoop&) = delete;

  uv_loop_t *handle();
  int run(uv_run_mode mode = UV_RUN_DEFAULT);
  void stop();
  void close();

  static 
  EventLoop* default_event_loop();
private:
  
  std::atomic_bool _is_run;
  uv_loop_t *_loop;
  LoopMode _mode;
};
} // namespace uvx

#endif