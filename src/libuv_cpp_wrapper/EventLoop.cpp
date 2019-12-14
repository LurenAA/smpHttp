#include "EventLoop.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include "FileLog.hpp"
#include <cassert>
#include <cstring>
#include <pthread.h>
#include "Tools.hpp"

using namespace std;
using namespace xx;
using namespace log4cpp;

Mutex EventLoop::mx;
/**
 * 初始化eventloop
 **/ 
EventLoop::EventLoop(LoopMode m)
  : _is_run(false), _mode(m)
{
  FileLog& file_log = FileLog::getInstance();
  int err;
  if(_mode == DEFAULT_LOOP) {
    /**
     * default_loop非线程安全
     **/ 
    EventLoop::mx.lock();
    _loop = uv_default_loop();
    EventLoop::mx.unlock();
    if(!_loop) {
      Tools& tl = Tools::getInstance();
      file_log.error("EventLoop uv_default_loop() error" + tl.get_strerror_r(errno), __func__, __FILE__, __LINE__);
      terminate();
    }
  }
  else {
    uv_loop_t* tmp_lp = new uv_loop_t();
    err = ::uv_loop_init(tmp_lp);
    if(err < 0) {
      Tools& tl = Tools::getInstance();
      file_log.error("EventLoop uv_loop_init() error: " + tl.get_strerror_r(errno), __func__, __FILE__, __LINE__);
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
  if(_mode == NEW_LOOP)
    delete _loop;
}

/**
 * uv_run
 **/ 
int EventLoop::run(uv_run_mode mode){
  _id = pthread_self();
  _is_run = true;
  int res = uv_run(_loop, mode);
  return res;
}

/**
 * 判断该事件循环是否在该线程运行
 **/ 
bool EventLoop::is_run_in_this_thread() const {
  if(!_is_run) 
    return false;
  pthread_t this_id = pthread_self();
  return this_id == _id;
}

/**
 * 结束事件循环
 **/ 
void EventLoop::stop() {
  if(!_is_run || _id == ULONG_MAX) 
    return ;
  uv_stop(_loop);
}

/**
 * 关闭loop
 **/ 
void EventLoop::close() {
  FileLog& lg = FileLog::getInstance();
  if(_is_run || is_active()) {
    lg.warn("close a running loop", __func__, __FILE__, __LINE__);
    return ;
  }
  int flag = uv_loop_close(_loop);
  if(flag < 0) {
    Tools& tl = Tools::getInstance();
    if(flag == UV_EBUSY) {
     lg.warn("cannot close a loop which is busy", __func__, __FILE__, __LINE__);
      return;
    } else {
      lg.error("uv_loop_close error : " + tl.get_uv_strerror_t(flag), __func__, __FILE__, __LINE__);
      terminate();
    }
  }
  _is_run = false;
}

/**
 * 判断是否active
 **/ 
bool EventLoop::is_active() const {
  return uv_loop_alive(_loop);
}

uv_loop_t *EventLoop::handle() {
  return _loop;
}