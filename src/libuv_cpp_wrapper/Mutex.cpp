#include "Mutex.hpp"
#include <exception>
#include <stdexcept>
#include <cstring>
#include <iostream>

using namespace std;
using namespace xx;

/**
 * pthread_mutex_lock
 **/
void Mutex::lock() {
  int sta = pthread_mutex_lock(&_mutex);
  if(sta != 0) {
    char* buf = new char[ERROR_BUF_SIZE];
    strerror_r(errno, buf, ERROR_BUF_SIZE);
    cout << buf << endl;
    terminate();
  }
}

/**
 * pthread_mutex_unlock
 **/ 
void Mutex::unlock() {
  auto& fl = FileLog::getInstance();
  auto& tl = Tools::getInstance();
  int sta = pthread_mutex_unlock(&_mutex);
  if(sta != 0) {
    fl.error("pthread_mutex_unlock error: " + tl.get_strerror_r(sta), __func__, __FILE__, __LINE__);
    terminate();
  } 
}

/**
 * pthread_trylock
 **/ 
bool Mutex::try_lock() {
  auto& fl = FileLog::getInstance();
  auto& tl = Tools::getInstance();
  int sta = pthread_mutex_trylock(&_mutex);
  if(sta != 0) {
    if(errno == EBUSY) {
      return false;
    } else {
      fl.error("pthread_mutex_unlock error: " + tl.get_strerror_r(sta), __func__, __FILE__, __LINE__);
      terminate();
    }
  }
  return true;
}

void Pthread::atfork(VoidVoidType prepare, VoidVoidType parent, VoidVoidType child) {
  int sta = ::pthread_atfork(prepare, parent, child);
  if(sta != 0) {
    auto& fl = FileLog::getInstance();
    auto& tl = Tools::getInstance();
    fl.error("pthread_atfork error: " + tl.get_strerror_r(sta), __func__, __FILE__, __LINE__);
    terminate();
  }
}