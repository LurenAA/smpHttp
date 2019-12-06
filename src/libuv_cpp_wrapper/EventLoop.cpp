#include "EventLoop.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include "FileLog.hpp"
#include <cassert>
#include <cstring>
#include <pthread.h>

using namespace std;
using namespace xx;
using namespace log4cpp;
static 
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
pthread_once_t once = PTHREAD_ONCE_INIT;

/**
 * 重新初始化mx
 **/ 
static
void reset_init_mx() {
  pthread_mutex_t mx_c = PTHREAD_MUTEX_INITIALIZER;
  ::memcpy(&mx, &mx_c, sizeof(mx_c));
}

/**
 * 重新初始化once
 **/ 
static
void reset_init_once() {
  pthread_once_t once_c = PTHREAD_ONCE_INIT;
  ::memcpy(&once,&once_c, sizeof once);
}

/**
 * 注册reset_init_mx,reset_init_once在fork时
 **/ 
static 
void reinit_at_fork() {
  if(pthread_atfork(nullptr,nullptr, reset_init_mx) || 
    pthread_atfork(nullptr, nullptr, reset_init_once)) {
      FileLog& log = FileLog::getInstance();
      std::string error_str = "pthread_atfork error in EventLoop: ";
      log.error(error_str);
      terminate();
    }
}

/**
 * 初始化eventloop
 **/ 
EventLoop::EventLoop(LoopMode m)
  : _mode(m), _is_run(false)
{
  if(pthread_once(&once, reinit_at_fork)) {
    auto& log = FileLog::getInstance();
    log.error("pthread_once in EventLoop error");
    terminate();
  }
  FileLog& file_log = FileLog::getInstance();
  int err;
  if(_mode == DEFAULT_LOOP) 
    /**
     * default_loop非线程安全
     **/ 
    pthread_mutex_lock(&mx);
    _loop = uv_default_loop();
    pthread_mutex_unlock(&mx);
    if(!_loop) {
      file_log.error("EventLoop uv_default_loop() error");
      terminate();
    }
  else {
    uv_loop_t* tmp_lp = new uv_loop_t();
    err = ::uv_loop_init(tmp_lp);
    if(err < 0) {
      file_log.error("EventLoop uv_loop_init() error");
      terminate();
    }
  }
}

/**
 * 返回默认事件循环
 **/ 
EventLoop* EventLoop::default_event_loop() {
  static EventLoop dl(DEFAULT_LOOP);
  return &dl;
}

EventLoop::~EventLoop() {
  assert(!_is_run);
  delete _loop;
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

uv_loop_t* Loop::getLoop() {
  return this->loop;
}