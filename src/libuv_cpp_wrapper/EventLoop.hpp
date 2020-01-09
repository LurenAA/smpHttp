#ifndef __EVENT_LOOP_HPP_
#define __EVENT_LOOP_HPP_
#include "uv.h"
#include <memory>
#include <atomic>
#include <climits>
#include "Mutex.hpp"

namespace xx{

class EventLoop{
public:
  enum LoopMode{
    NEW_LOOP,
    DEFAULT_LOOP
  };

  explicit EventLoop(LoopMode m = DEFAULT_LOOP);
  ~EventLoop();
  EventLoop(const EventLoop&) = delete;
  EventLoop& operator=(const EventLoop&) = delete;

  uv_loop_t *handle();
  int run(uv_run_mode mode = UV_RUN_DEFAULT);
  bool is_run_in_this_thread() const;
  void stop();
  void close();
  bool is_active() const;

  static 
  EventLoop* default_event_loop();
private:
  pthread_t _id = ULONG_MAX;
  std::atomic_bool _is_run;
  uv_loop_t *_loop;
  LoopMode _mode;

  static 
  Mutex mx;
};
} // namespace uvx

#endif