#include "Packet.hpp"
#include "Util.hpp"
#include <iostream>
#include <cstring>
#include "FileLog.hpp"
#include "Tools.hpp"
using namespace std;
using namespace xx;

#define CRLF "\r\n"
#define CHUNK_END "0\r\n\r\n"

/**
 * 添加头部
 **/ 
void Packet::addHeader(const std::string& name,const  std::string& value) {
  if(mode == CHUNKED || name == "Transfer-Encoding"){
    auto& fl = FileLog::getInstance();
    fl.error("cannot set Transfer-Encoding \
    directly or set headers to a chunked datagram", __func__ , __FILE__, __LINE__);
    return;
  }
  headers.insert({name, value});
}

/**
 * 转化成数据报
 **/ 
std::string Packet::get() {
#define AXX(nu, sm, des) \
  if(nu == static_cast<int>(status)) \
    res += #sm; \

  string res = "";
  bool if_set_content_length = false; //determine if set content-length
  if(mode != CHUNKED) {
    res += translate_version_to_string() + " ";
    res += to_string(static_cast<unsigned long long>(status)) + " ";
    HTTP_STATUS_MAP(AXX);
    res += CRLF;
    for(auto x : headers) {
      res += x.first + ":" + x.second + CRLF;
      if(!strcasecmp(x.first.c_str(), "Content-Length"))
        if_set_content_length = true;
    }
    if(mode == NORMAL && !if_set_content_length) 
      res += "Content-Length:" + to_string(message.size()) + CRLF;
    else if(mode == CHUNKED_START)
      res += "Transfer-Encoding:chunked\r\n";  
  } 

  if(mode != CHUNKED) 
    res += CRLF; //below is the portion for message body
  if(mode == NORMAL) {
    res += message;
  } else {
    res += chunk_data(message);
  }

  if(is_last_chunked)
    res += CHUNK_END;
#undef AXX
  return res;
}

/**
 * chunked格式
 **/ 
std::string Packet::chunk_data(std::string s) const{
  char chunkSize[10] = "";
  sprintf(chunkSize, "%x", static_cast<unsigned int>(s.size()));
  string res(chunkSize);
  res += "\r\n" + s + "\r\n";
  return res;
}

/**
 * 添加content-type
 **/ 
void Packet::setContentType(const std::string &s) {
  string ext = Util::getExt(s);
  if(!ext.size()) 
    return;
  string mime = Util::getMime(ext);
  if(!mime.size()) 
    return ;
  addHeader("Content-Type", mime);
}

/**
 * 将httpversion从enum转化成string
 **/ 
std::string Packet::translate_version_to_string() const {
  if(http_version == HTTP_09)
    return "HTTP/0.9";
  else if (http_version == HTTP_10)
    return "HTTP/1.0";
  else 
    return "HTTP/1.1";
}

/**
 * 标记已经是最后一个chunked的包了，
 * 在get（）的时候会添加最后一个chunked包的结尾
 **/ 
void Packet::setLastChunked(bool b) {
  if(mode != NORMAL) 
    is_last_chunked = b;
}

void Packet::addMessage(const char* s, int size) {
  message += string(s, size);
}