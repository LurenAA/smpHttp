#ifndef __LOOP_HPP_
#define __LOOP_HPP_
#include "uv.h"
#include <memory>

namespace uvx{
enum LoopState{
  STOPPED = 191,
  RUNNING,
  CLOSED
};

class Loop{
friend class Tcp;
public:
  Loop();
  ~Loop();
  void setLoop(uv_loop_t *);
  uv_loop_t *getLoop();
  int run(uv_run_mode mode = UV_RUN_DEFAULT);
  void stop();
  void close();

private:
  LoopState state;
  uv_loop_t *loop;
};
} // namespace uvx

#endif