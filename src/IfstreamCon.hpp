#ifndef __IfstreamCon_HPP
#define __IfstreamCon_HPP
#include <fstream>
#include <string>

namespace smpHttp {
  class IfstreamCon
  {
  private:
    std::ifstream fs;
    std::string path;
  public:
    std::ifstream& getFs() {return fs;}
    bool open(std::string path); 
    void close();
    bool is_open() { return fs.is_open();}
    ~IfstreamCon();
  };
}
#endif //__IfstreamCon_HPP