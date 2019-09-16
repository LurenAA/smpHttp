#ifndef __UTIL_HPP__
#define __UTIL_HPP__
#include <string>

namespace smpHttp {
  class Util {
    public:
      static void trim(std::string& s);
      static std::string getRootPath();
      static std::string getMime(const std::string&);
      static std::string getExt(const std::string &);
  };
}
#endif //__UTIL_HPP__