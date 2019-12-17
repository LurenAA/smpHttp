#ifndef __UTIL_HPP__
#define __UTIL_HPP__
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "mysqlx/xdevapi.h"
#include "CCommon.hpp"
#include "Tools.hpp"

namespace xx {
  class Util {
    public:
      static void trim(std::string& s);
      static std::string getRootPath(); 
      static std::string getMime(const std::string&);
      static std::string getExt(const std::string &);
      static bool starts_with(const std::string& source, const std::string& );
      static std::string join(const std::vector<std::string>& vec, char j);
      static std::vector<std::string> split(const std::string& str, char j);
      static std::string utf16Toutf8(const mysqlx::Value& v);
      static bool isBase64(const std::string& s);
      static std::string getStaticDirnameUrl(const std::string& host, int port);
      static std::string get_uv_strerror_t(int errno_r);
      static std::string get_strerror_r(int errno_r);
      static std::string conbine_prefix_and_path(std::string, std::string);
      static int get_thread_num();
      static bool set_thread_num(int num);
    private:
      static std::shared_ptr<std::map<std::string, std::string>> mimes;
  };
}
#endif //__UTIL_HPP__