#ifndef __Packet_HPP_
#define __Packet_HPP_
#include <string>
#include <map>
#include "CCommon.hpp"

namespace xx {
  class Packet {
    public:
      virtual ~Packet() {}
      void setHttpVersion(HttpVersion version) {http_version = version;}
      void setMode(TransMode amode) {mode = amode;}
      void setHttpStatus(HttpStatus astatus) {status = astatus;}
      void addHeader(const std::string& name,const  std::string& value);
      std::string get();
      void setContentType(const std::string &s);
      void addMessage(const std::string& s) {message += s;}
      void setLastChunked(bool b);
      void setLastChunked() {is_last_chunked = true;}
      const TransMode getMode() {return mode;}
      size_t getMessageSize() const {return message.size();}

    private:
      std::map<std::string, std::string> headers;
      TransMode mode = NORMAL;//正常数据报还是CHUNKED
      HttpVersion http_version = HTTP_11;
      HttpStatus status = HTTP_OK;
      bool is_last_chunked = false; //是否添加chunked的最后的结束部分
      std::string message = "";

      std::string translate_version_to_string() const;
      std::string chunk_data(std::string s) const; //在get（）中使用
  };
}
#endif //__URL_HPP_