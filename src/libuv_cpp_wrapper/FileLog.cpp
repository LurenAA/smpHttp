#include "FileLog.hpp"
#include "pthread.h"
#include <functional>
#include <cstring>
#include <cassert>

#define METHOD_FUNCTION_HELPER(method) \
  void  \
  FileLog::method(const std::string& str, const std::string& _func , const std::string _file , int _line ) { \
  LogFunc_cb f = (void(Category::*)(const std::string&))&Category::method; \
  invoke_log(str, _func, _file, _line,f); \
  }
  
using namespace std;
using namespace xx;
using namespace std::placeholders;
Mutex FileLog::mx;
/**
 * 读取日志配置的路径
 **/ 
std::string FileLog::init_path = "./file_log.ini";
FileLog* FileLog::singleton = nullptr;


/**
 * 注册FileLog的初始化函数
 **/ 
void 
FileLog::init() {
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
 * 获取FileLog的单例
 **/ 
FileLog& 
FileLog::getInstance() {
  if(!FileLog::singleton) {
    FileLog::mx.lock();
    if(!FileLog::singleton) {
      init();
    }
    FileLog::mx.unlock();
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
FileLog::invoke_log(const std::string& s, const std::string& _func , const std::string _file , int _line , LogFunc_cb f) {
  Category& root = Category::getRoot();
  string str = _func;
  str += " : ";
  str += _file;
  str += " : ";
  str += to_string(_line);
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