#include "IfstreamCon.hpp"

using namespace std;
using namespace smpHttp;

bool IfstreamCon::open(std::string apath){
  if(apath.size()) {
    path = apath;
    fs.open(apath);
    return is_open();
  }
  return false;
}

IfstreamCon::~IfstreamCon() {
  close();
}

void IfstreamCon::close() {
  if(is_open()) 
    fs.close();
}