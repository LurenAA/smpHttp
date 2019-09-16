#include "Util.hpp"
#include <unistd.h>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace smpHttp;

void Util::trim(std::string& s) {
  if( !s.empty() )
  {
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1, s.size());
  }
}

/**
 * when I run it in "/home/ele/smpHttp/build",
 * the cwd is "/home/ele/smpHttp/build",
 * but when I debug in vs code using GDB and ssh, 
 * getcwd(NULL, 0) return "/home/ele/smpHttp".
 * so I cut the "/build" in the string.
 **/ 
std::string Util::getRootPath() {
#define ERR_STR "/build"
  static string path(getcwd(nullptr, 0));
  static bool flag = false;
  if(!flag) {
    string comparseOne(ERR_STR);
    auto ri1 = comparseOne.rbegin();
    auto ri = path.rbegin();
    int t = 6;
    for(;t > 0; --t){
      if(*ri == *ri1) {
        ++ri;
        ++ri1;
      } else {
        break;
      }
    }
    if(t == 0) {
      path.erase(path.end() - 6, path.end());
    }
    flag = true;
  }
  return path;
#undef ERR_STR
}

string Util::getMime(const std::string& s){
  string res;

  return res;
}

std::string Util::getExt(const std::string &s) {
  string res;

  return res;
}
