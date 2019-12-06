#include "Tools.hpp"
#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <cstring>

using namespace std;
using namespace xx;
static 
pthread_once_t once = PTHREAD_ONCE_INIT;
static
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

Tools* Tools::singleton = nullptr;

/**
 * fork时初始化once和mx
 **/ 
static
void reinit_in_fork() {
pthread_once_t once_c = PTHREAD_ONCE_INIT;
::memcpy(&once,&once_c, sizeof once_c);
pthread_mutex_t mx_c = PTHREAD_MUTEX_INITIALIZER;
::memcpy(&mx,&mx_c, sizeof mx);
}

/**
 * 注册reinit_in_fork
 **/ 
static
void init_once() {
  if(pthread_atfork(nullptr, nullptr, reinit_in_fork)) {
    char* buf = new char[100];
    strerror_r(errno, buf, 100);
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
  delete singleton;
}