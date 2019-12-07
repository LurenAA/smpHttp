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
      Tools& tl = Tools::getInstance();
      std::string error_str = "pthread_atfork error in EventLoop: " + tl.get_strerror_r(errno);
      log.error(error_str);
      terminate();
    }
}

/**
 * 初始化eventloop
 **/ 
EventLoop::EventLoop(LoopMode m)
  : _is_run(false), _mode(m)
{
  if(pthread_once(&once, reinit_at_fork)) {
    auto& log = FileLog::getInstance();
    Tools& tl = Tools::getInstance();
    log.error("pthread_once in EventLoop error: " + tl.get_strerror_r(errno));
    terminate();
  }
  FileLog& file_log = FileLog::getInstance();
  int err;
  if(_mode == DEFAULT_LOOP) {
    /**
     * default_loop非线程安全
     **/ 
    pthread_mutex_lock(&mx);
    _loop = uv_default_loop();
    pthread_mutex_unlock(&mx);
    if(!_loop) {
      Tools& tl = Tools::getInstance();
      file_log.error("EventLoop uv_default_loop() error" + tl.get_strerror_r(errno));
      terminate();
    }
  }
  else {
    uv_loop_t* tmp_lp = new uv_loop_t();
    err = ::uv_loop_init(tmp_lp);
    if(err < 0) {
      Tools& tl = Tools::getInstance();
      file_log.error("EventLoop uv_loop_init() error: " + tl.get_strerror_r(errno));
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
  int res = uv_run(_loop, mode);
  _id = pthread_self();
  _is_run = true;
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
    lg.warn("close a running loop");
    return ;
  }
  int flag = uv_loop_close(_loop);
  if(flag < 0) {
    Tools& tl = Tools::getInstance();
    if(flag == UV_EBUSY) {
     lg.warn("warning: cannot close a loop which is busy");
      return;
    } else {
      lg.error("uv_loop_close error : " + tl.get_uv_strerror_t(flag));
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