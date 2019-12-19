#include "Fs.hpp"
#include <iostream>
#include "EventLoop.hpp"
#include "FileLog.hpp"
#include "Tools.hpp"
#include "cstring"

using namespace std;
using namespace xx;

void Fs::clean_up()
{
  uv_fs_req_cleanup(req());
}

Fs::Fs(EventLoop &_lp) :_callback([](uv_fs_t *req1) {
    Fs *fs = reinterpret_cast<Fs *>(req1->data);
    fs->setState(FsState::idle);
    if(!fs) 
      return ;
    fs->invokeCb();
  }), _lp(_lp), cb(nullptr)
{
  ::memset(&_fs, 0, sizeof _fs);
  _fs.data = this;
}

uv_fs_t *Fs::req()
{
  return &_fs;
}

/**
 * open
 **/
int Fs::open(const std::string &path, int flags, int mode)
{
  if(state != FsState::idle) {
    return false;
  }
  this->setState(FsState::busy);
  clean_up();
  int flag = uv_fs_open(_lp.handle(), req(), path.c_str(), flags, mode, _callback);
  if(flag < 0) {
    auto & fl = FileLog::getInstance();
    fl.error("open error: " + Tools::getInstance().get_uv_strerror_t(flag), __func__, __FILE__ , __LINE__);
    return false;
  }
  return 1;
}

/**
 * close
 **/
int Fs::close(uv_file file) {
  if(state != FsState::idle) {
    return false;
  }
  this->setState(FsState::busy);
  clean_up();
  int flag = uv_fs_close(_lp.handle(), req(), file, _callback);
  if(flag < 0) {
    auto & fl = FileLog::getInstance();
    fl.error("close error: " + Tools::getInstance().get_uv_strerror_t(flag), __func__, __FILE__ , __LINE__);
    return false;
  }
  return 1;
}

/**
 * pread
 **/ 
int Fs::read(uv_file file,  int64_t offset) {
  if(state != FsState::idle) {
    return false;
  }
  this->setState(FsState::busy);
  clean_up();
  int flag = uv_fs_read(_lp.handle(), req(), file, &buf, 1, offset, _callback);
  if(flag < 0) {
    auto & fl = FileLog::getInstance();
    fl.error("read error: " + Tools::getInstance().get_uv_strerror_t(flag), __func__, __FILE__ , __LINE__);
    return false;
  }
  return 1;
}

/**
 * pwrite
 **/
int Fs::write(uv_file file, int64_t offset) {
  if(state != FsState::idle) {
    return false;
  }
  this->setState(FsState::busy);
  clean_up();
  int flag = uv_fs_write(_lp.handle(), req(), file, &buf, 1, offset, _callback);
  if(flag < 0) {
    auto & fl = FileLog::getInstance();
    fl.error("open error: " + Tools::getInstance().get_uv_strerror_t(flag), __func__, __FILE__ , __LINE__);
    return false;
  }
  return 1;
} 

/**
 * uv_fs_stat
 **/ 
 int Fs::stat(const std::string& s) {
   if(state != FsState::idle) {
    return false;
  }
  this->setState(FsState::busy);
  clean_up();
  int flag = uv_fs_stat(_lp.handle(), req(), s.c_str() , _callback);
  if(flag < 0) {
    auto & fl = FileLog::getInstance();
    fl.error("open error: " + Tools::getInstance().get_uv_strerror_t(flag), __func__, __FILE__ , __LINE__);
    return false;
  }
  return 1;
 }
/**
 * 设置回调
 **/ 
void Fs::setCb(FsType f)
{
  cb = f;
}

/**
 * 调用回调
 **/ 
void Fs::invokeCb()
{
  if (this->cb)
  {
    cb(this);
  }
}

/**
 * 设置状态
 **/ 
void Fs::setState(FsState astate) {
  mx_for_state.lock();
  this->state = astate;
  mx_for_state.unlock();
}

/**
 * 获取状态
 **/ 
Fs::FsState Fs::getState() {
  FsState ux;
  mx_for_state.lock();
  ux = this->state;
  mx_for_state.unlock();
  return ux;
}

/**
 * 为缓存分配内存
 **/ 
int Fs::enable_buf(unsigned int size) {
  mx_for_new.lock();
  if(is_use_buf)
    return false; 
  is_use_buf = true;
  buf.base = new char[size]();
  buf.len = size;
  mx_for_new.unlock();
  return size;
}
      
/**
 * 释放buf的内存
 **/ 
void Fs::release_buf() {
  mx_for_new.lock();
  if(!is_use_buf) 
    return ;
  delete [] buf.base;
  is_use_buf = false;
  mx_for_new.unlock();
}

Fs::~Fs() {
  release_buf();
}