#include "Handle.hpp"
#include <iostream>
#include <cassert>
#include "FileLog.hpp"

using namespace std;
using namespace xx;

/**
 *  判断handle是否关闭
 **/ 
bool Handle::is_closing() const {
  if(!_handle) 
    return false;
  return static_cast<bool>(uv_is_closing(_handle));
}

Handle::~Handle(){
  assert(is_closing());
  delete _handle;
}

/**
 * 关闭handle
 **/ 
void Handle::close() {
  if(is_closing() | !_handle)
    return;
  uv_close(_handle, [](uv_handle_t* handle) {
    FileLog& log = FileLog::getInstance();
    log.debug("handle close");
    Handle* had = static_cast<Handle*>(handle->data);
    had->invokeCloseCb();
  });
}

/**
 * 判断handle是否时active
 **/ 
bool Handle::is_active() const{
  if(!_handle)
    return false;
  return uv_is_active(_handle);
}

/**
 * 初始化Handle，接受各类handle
 **/ 
Handle::Handle(HandleType t) :
  _type(t)
{
#define check_type(name, type)  \
  case name :  \
    _handle = reinterpret_cast<uv_handle_t*>(new type()); \
  break;

switch (t)
{
HANDLE_METHODS(check_type)
default:
  cout << "Handle error " << endl;
  terminate();
  break;
}

_handle->data = static_cast<void*>(this);
#undef check_type
}

/**
 * 设置close_cb
 **/ 
void Handle::setCloseCb(CloseCbType cb) {
  close_cb = cb;
}

/**
 * 在uv_close回调函数中执行close_cb
 **/ 
void Handle::invokeCloseCb() {
  if(close_cb)
    close_cb(this);
}

/**
 * 返回原生的handle接口
 **/ 
uv_handle_t* Handle::handle() {
  return _handle;
}
