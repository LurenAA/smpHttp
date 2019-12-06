#include "FileLog.hpp"
#include "pthread.h"
#include <functional>
#include <cstring>
#include <cassert>

#define METHOD_FUNCTION_HELPER(method) \
  void  \
  FileLog::method(const std::string& str) { \
  LogFunc_cb f = (void(Category::*)(const std::string&))&Category::method; \
  invoke_log(str, f); \
  }
  
using namespace std;
using namespace xx;
using namespace std::placeholders;
static pthread_once_t init_once = PTHREAD_ONCE_INIT; //用于文件初始化
static pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER; //用于singleton

/**
 * 读取日志配置的路径
 **/ 
std::string FileLog::init_path = "./file_log.ini";
FileLog* FileLog::singleton = nullptr;

/**
 * 在fork时重新初始化init_once
 **/
static void 
reset_init_once() {
  pthread_once_t child_once = PTHREAD_ONCE_INIT;
  ::memcpy(&init_once, &child_once, sizeof(child_once));
}

/**
 * 初始化log4cpp函数
 **/ 
static void 
_init() {
  if(pthread_atfork(nullptr, nullptr, &reset_init_once)) {
    cout << "fatal: pthread_atfork error" << endl;
    terminate();
  }
  try
  {
    if(FileLog::getInitPath().empty()) 
      return ;
    log4cpp::PropertyConfigurator::configure(FileLog::getInitPath());
  }
  catch(log4cpp::ConfigureFailure& f)
  {
    std::cout<< "Configure Problem : "<< f.what()<< std::endl;
    terminate();
  }
}

/**
 * 注册FileLog的初始化函数
 **/ 
void 
FileLog::init() {
  if(pthread_once(&init_once, _init))
    terminate();
}

/**
 * 获取FileLog的单例
 **/ 
FileLog& 
FileLog::getInstance() {
  if(!FileLog::singleton) {
    pthread_mutex_lock(&mx);
    if(!FileLog::singleton) {
      init();
    }
    pthread_mutex_unlock(&mx);
  }
  return *singleton;
}

/**
 * init_path的get
 **/ 
const std::string& 
FileLog::getInitPath(){
  return FileLog::init_path;
}

/**
 * init_path的set
 **/ 
void 
FileLog::setInitPath(const std::string& s) {
  FileLog::init_path = s;
}

/**
 * error、debug、fatal、crit这些函数相同的部分提取出来
 * */
void 
FileLog::invoke_log(const std::string& s, LogFunc_cb f) {
  Category& root = Category::getRoot();
  string str = __func__;
  str += " : ";
  str += __FILE__;
  str += " : ";
  str += __LINE__;
  str += " : \n";
  str += s;
  (root.*f)(str);
}

METHOD_FUNCTION_HELPER(info)
METHOD_FUNCTION_HELPER(debug)
METHOD_FUNCTION_HELPER(warn)
METHOD_FUNCTION_HELPER(error)

FileLog::~FileLog() {
  shutdown();
}