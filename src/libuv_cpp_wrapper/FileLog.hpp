#ifndef __LOG_HPP_
#define __LOG_HPP_
#include <string>
#include "log4cpp/Category.hh"
#include "log4cpp/PropertyConfigurator.hh"
#include <functional>
#include <cassert>
/**
 * 使用singleton模式，这样一来可以满足我要从文件初始化log4cpp的需求，
 * 而来可以保证程序结束时自动销毁（如果我使用静态方法，那么最后我需要自己来执行shutdown函数）
 **/
namespace xx {
  using LogFunc_cb = void(log4cpp::Category::*)(const std::string&);
  class FileLog : public log4cpp::Category{
    public:
      static FileLog& getInstance();
      static const std::string& getInitPath();
      static void setInitPath(const std::string&); 

      void debug(const std::string&, const std::string& _func, const std::string _file , int _line );
      void error(const std::string&, const std::string& _func, const std::string _file , int _line );
      void warn(const std::string&, const std::string& _func, const std::string _file , int _line );
      void info(const std::string&, const std::string& _func, const std::string _file , int _line );
      
      FileLog() = delete;
      FileLog(const FileLog&) = delete;
      FileLog& operator=(const FileLog&) = delete;
      ~FileLog();
    private: 
      static void invoke_log(const std::string& s, const std::string& _func , const std::string _file , int _line , LogFunc_cb f);
      static FileLog* singleton;
      static std::string init_path; 
      static void init();
  };
}

#endif