#include "IfstreamCon.hpp"
#include <iostream>

using namespace std;
using namespace smpHttp;

bool IfstreamCon::open(std::string apath){
  if(apath.size()) {
    path = apath;
    fs.open(apath, ios::binary);
    bool status = is_open();
    if(status) {
      fs.seekg(0, ios::end);
      asize = fs.tellg();
      fs.seekg(0, ios::beg);
      return true;
    }
    return false; 
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

IfstreamCon::size_type IfstreamCon::read(std::string& s, size_type max_size){
  char buf[max_size + 1] = ""; //the additional one space is very important
  size_type remain = asize - has_read;
  // cout << remain << endl;
  if(remain <= max_size) {
    fs.read(buf, remain);
    has_read += remain;
    s.assign(buf);
    return remain;
  } else {
    fs.read(buf, max_size);
    has_read += max_size;
    s.assign(buf, max_size);
    // cout << s.size() << endl;
    return max_size;
  }
}