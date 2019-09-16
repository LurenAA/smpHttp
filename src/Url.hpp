#ifndef __URL_HPP_
#define __URL_HPP_
#include <string>
#include <map>

namespace smpHttp {
  class Url {
    public:
      void addHeader(const std::string& name,const  std::string& value);
      std::string get();
      void setContentType(const std::string &s);
      static std::string chunk_data(std::string s);
    private:
      std::map<std::string, std::string> headers;
      std::string url;
  };
}
#endif //__URL_HPP_