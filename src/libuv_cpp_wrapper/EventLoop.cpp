#include "EventLoop.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace xx;

EventLoop::EventLoop(LoopMode m)
  : _mode(m), _is_run(false)
{
  if(_mode == DEFAULT_LOOP) 
    _loop = uv_default_loop();
  else {
    uv_loop_t* tmp_lp = new uv_loop_t();
    ::uv_loop_init(tmp_lp);

  }
}

EventLoop* EventLoop::default_event_loop() {
  static EventLoop dl(DEFAULT_LOOP);
  return &dl;
}

int Loop::run(uv_run_mode mode){
  static vector<int> mode_lib({UV_RUN_DEFAULT, UV_RUN_ONCE, UV_RUN_NOWAIT});
  // make sure the variable "mode" is a signifcant value 
  if(find(mode_lib.cbegin(), mode_lib.cend(), mode) == mode_lib.cend())
    mode = UV_RUN_DEFAULT;
  state = RUNNING;
  int res = uv_run(loop, mode);
  state = STOPPED;
  return res;
}

void Loop::stop() {
  if(state != RUNNING){
    cerr << "warning: can not stop a loop which isn`t running" << endl;
    return;
  }
  state = STOPPED;
  uv_stop(loop);
}

void Loop::close() {
  if(state != STOPPED) {
    cerr << "warning: cannot close a running loop" << endl;
    return ;
  }
  int flag = uv_loop_close(loop);
  if(flag == UV_EBUSY) {
    cerr << "warning: cannot close a loop which is busy" << endl;
    return;
  }
  state = CLOSED;
}

Loop::~Loop() {
  if(state == RUNNING) 
    cerr << "error: delete a running loop" << endl;
  if(state == STOPPED)
    close();
}

uv_loop_t* Loop::getLoop() {
  return this->loop;
}