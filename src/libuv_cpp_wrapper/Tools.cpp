#include "Tools.hpp"
#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <cstring>
#include "uv.h"

using namespace std;
using namespace xx;
static 
pthread_once_t once = PTHREAD_ONCE_INIT;
static
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
static
pthread_mutex_t mx_strerror = PTHREAD_MUTEX_INITIALIZER;
static 
pthread_mutex_t mx_uv_strerror = PTHREAD_MUTEX_INITIALIZER;

Tools* Tools::singleton = nullptr;

#define ERROR_BUF 1024
/**
 * fork时初始化once和mx
 **/ 
static
void reinit_in_fork() {
pthread_once_t once_c = PTHREAD_ONCE_INIT;
::memcpy(&once,&once_c, sizeof once_c);
pthread_mutex_t mx_c = PTHREAD_MUTEX_INITIALIZER;
::memcpy(&mx,&mx_c, sizeof mx);
::memcpy(&mx_strerror,&mx_c, sizeof mx);
::memcpy(&mx_uv_strerror,&mx_c, sizeof mx);
}

/**
 * 注册reinit_in_fork
 **/ 
static
void init_once() {
  if(pthread_atfork(nullptr, nullptr, reinit_in_fork)) {
    char* buf = new char[ERROR_BUF];
    strerror_r(errno, buf, ERROR_BUF);
    cout << "error : " << buf << endl;
    terminate();
  }
}

/**
 * 获取Tools实例
 **/ 
Tools& Tools::getInstance() {
  if(pthread_once(&once, init_once)) {
    char* buf = new char[100];
    strerror_r(errno, buf, 100);
    cout << "error : " << buf << endl;
    terminate();
  }
  if(!singleton) {
    pthread_mutex_lock(&mx);
    if(!singleton)
      singleton = new Tools();
    pthread_mutex_unlock(&mx);
  } 
  return *singleton;
}

Tools::~Tools() {
}

/**
 * 线程安全的strerror_r
 **/ 
std::string Tools::get_strerror_r(int errno_r) {
  static std::string s = "";
  static char buf[ERROR_BUF];
  pthread_mutex_lock(&mx_strerror);
  strerror_r(errno_r, buf, ERROR_BUF);
  s.assign(buf);
  pthread_mutex_unlock(&mx_strerror);
  return s;
}

/**
 * 线程安全的uv_strerror_r
 **/ 
std::string Tools::get_uv_strerror_t(int errno_r) {
  static std::string s = "";
  static char buf[ERROR_BUF];
  pthread_mutex_lock(&mx_uv_strerror);
  uv_strerror_r(errno_r, buf, ERROR_BUF);
  s.assign(buf);
  pthread_mutex_unlock(&mx_uv_strerror);
  return s;
}