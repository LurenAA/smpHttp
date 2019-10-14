#ifndef __UTIL_HPP__
#define __UTIL_HPP__
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace smpHttp {
  class Util {
    public:
      static void trim(std::string& s);
      static std::string getRootPath(); 
      static std::string getMime(const std::string&);
      static std::string getExt(const std::string &);
      static bool starts_with(const std::string& source, const std::string& );
      static std::string join(const std::vector<std::string>& vec, char j);
      static std::vector<std::string> split(const std::string& str, char j);
    private:
      static std::shared_ptr<std::map<std::string, std::string>> mimes;
  };
}
#endif //__UTIL_HPP__