#include "Packet.hpp"
#include "Util.hpp"
#include <iostream>

using namespace std;
using namespace smpHttp;

void Packet::addHeader(const std::string& name,const  std::string& value) {
  if(mode == CHUNKED || name == "Transfer-Encoding"){
    cout << "log: cannot set Transfer-Encoding \
    directly or set headers to a chunked datagram" << endl;
    return;
  }
  headers.insert({name, value});
}

std::string Packet::get() {
  string res = "HTTP/1.1 200 OK\r\n";
  for(auto iter = headers.cbegin(); iter != headers.cend(); iter++) {
    res += iter->first + ":" + iter->second + "\r\n";
  }
  res += "\r\n";
  return res;
}

// std::string Packet::chunk_data(std::string s) {
//   char chunkSize[10] = "";
//   sprintf(chunkSize, "%x", static_cast<unsigned int>(s.size()));
//   string res(chunkSize);
//   res += "\r\n" + s + "\r\n";
//   return res;
// }

void Packet::setContentType(const std::string &s) {
  string ext = Util::getExt(s);
  if(!ext.size()) 
    return;
  string mime = Util::getMime(ext);
  if(!mime.size()) 
    return ;
  addHeader("Content-Type", mime);
}

std::string Packet::translate_version_to_string() {
  if(http_version == HTTP_09)
    return "HTTP/0.9";
  else if (http_version == HTTP_10)
    return "HTTP/1.0";
  else 
    return "HTTP/1.1";
}

