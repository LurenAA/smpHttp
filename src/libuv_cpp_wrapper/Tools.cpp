#include "Tools.hpp"
#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <cstring>
#include "uv.h"

using namespace std;
using namespace xx;
Mutex Tools::mx, Tools::mx_sr, Tools::mx_uv_sr;

Tools* Tools::singleton = nullptr;

/**
 * 获取Tools实例
 **/ 
Tools& Tools::getInstance() {
  if(!singleton) {
    mx.lock();
    if(!singleton)
      singleton = new Tools();
    mx.unlock();
  } 
  return *singleton;
}

/**
 * 线程安全的strerror_r
 **/ 
std::string Tools::get_strerror_r(int errno_r) {
  static std::string s = "";
  static char buf[ERROR_BUF_SIZE];
  mx_sr.lock();
  strerror_r(errno_r, buf, ERROR_BUF_SIZE);
  s.assign(buf);
  mx_sr.unlock();
  return s;
}

/**
 * 线程安全的uv_strerror_r
 **/ 
std::string Tools::get_uv_strerror_t(int errno_r) {
  static std::string s = "";
  static char buf[ERROR_BUF_SIZE];
  mx_uv_sr.lock();
  uv_strerror_r(errno_r, buf, ERROR_BUF_SIZE);
  s.assign(buf);
  mx_uv_sr.unlock();
  return s;
}