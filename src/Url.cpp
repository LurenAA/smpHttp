#include "Url.hpp"
#include "Util.hpp"

using namespace std;
using namespace smpHttp;

void Url::addHeader(const std::string& name,const  std::string& value) {
  headers.insert({name, value});
}

std::string Url::get() {
  string res = "HTTP/1.1 200 OK\r\n";
  for(auto iter = headers.cbegin(); iter != headers.cend(); iter++) {
    res += iter->first + ":" + iter->second + "\r\n";
  }
  res += "\r\n";
  return res;
}

std::string Url::chunk_data(std::string s) {
  char chunkSize[10] = "";
  sprintf(chunkSize, "%x", static_cast<unsigned int>(s.size()));
  string res(chunkSize);
  res += "\r\n" + s + "\r\n";
  return res;
}

void Url::setContentType(const std::string &s) {
  string ext = Util::getExt(s);
  if(!ext.size()) 
    return;
  string mime = Util::getMime(ext);
  if(!mime.size()) 
    return ;
  addHeader("Content-Type", mime);
}