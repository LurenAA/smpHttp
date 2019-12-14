#include "Mutex.hpp"
#include <exception>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include "Common.hpp"

using namespace std;
using namespace xx;

/**
 * pthread_mutex_lock
 **/
void Mutex::lock() {
  int sta = pthread_mutex_lock(&_mutex);
  if(sta != 0) {
    char* buf = new char[ERROR_BUF_SIZE]();
    strerror_r(sta, buf, ERROR_BUF_SIZE);
    cout << "pthread_mutex_lock error" << buf << endl;
    terminate();
  }
}

/**
 * pthread_mutex_unlock
 **/ 
void Mutex::unlock() {
  int sta = pthread_mutex_unlock(&_mutex);
  if(sta != 0) {
    char* buf = new char[ERROR_BUF_SIZE]();
    strerror_r(sta, buf, ERROR_BUF_SIZE);
    cout << "pthread_mutex_unlock error" << buf << endl;
    terminate();
  } 
}

/**
 * pthread_trylock
 **/ 
bool Mutex::try_lock() {
  int sta = pthread_mutex_trylock(&_mutex);
  if(sta != 0) {
    if(sta == EBUSY) {
      return false;
    } else {
      char* buf = new char[ERROR_BUF_SIZE]();
      strerror_r(sta, buf, ERROR_BUF_SIZE);
      cout << "try_lock error" << buf << endl;
      terminate();
    }
  }
  return true;
}

void Pthread::atfork(VoidVoidType prepare, VoidVoidType parent, VoidVoidType child) {
  int sta = ::pthread_atfork(prepare, parent, child);
  if(sta != 0) {
    char* buf = new char[ERROR_BUF_SIZE]();
    strerror_r(sta, buf, ERROR_BUF_SIZE);
    cout << "pthread_atfork error" << buf << endl;
    terminate();
  }
}