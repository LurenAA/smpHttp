#include "Packet.hpp"
#include "Util.hpp"
#include <iostream>
#include <cstring>

using namespace std;
using namespace smpHttp;

#define CRLF "\r\n"

void Packet::addHeader(const std::string& name,const  std::string& value) {
  if(mode == CHUNKED || name == "Transfer-Encoding"){
    cout << "log: cannot set Transfer-Encoding \
    directly or set headers to a chunked datagram" << endl;
    return;
  }
  headers.insert({name, value});
}

std::string Packet::get() {
#define AXX(nu, sm, des) \
  if(nu == static_cast<int>(status)) \
    res += #sm; \

  string res = "";
  bool if_set_content_length = false; //determine if set content-length
  if(mode != CHUNKED) {
    res += translate_version_to_string() + " ";
    res += static_cast<int>(status) + " ";
    HTTP_STATUS_MAP(AXX);
    res += CRLF;
    for(auto x : headers) {
      res += x.first + ":" + x.second + CRLF;
      if(!strcasecmp(x.first.c_str(), "Content-Length"))
        if_set_content_length = true;
    }
    if(mode == NORMAL) 
      res += "Content-Length:" + to_string(message.size()) + CRLF;
      
  } 

  res += CRLF; //below is the portion for message body

  return res;
#undef AXX(nu, sm, des)
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

