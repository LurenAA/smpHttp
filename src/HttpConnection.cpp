#include <iostream>
#include "HttpConnection.hpp"
#include <cstring>

using namespace std;
using namespace smpHttp;
using namespace uvx;

void HttpConnection::onStartRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  if(nread < 0 || (remain == 0 && nread == 0)) {
    if(remain > 0) {
      cout << "half end : " << remain << endl;
    }
    cout << "log: close a connection" << endl;
    close();
    return ;
  }
  if(!method.size() && remain == INT_MIN) { 
    //firstly enter
    char* method_end = strstr(buf->base, " ");
    if(!method_end) {
      cout << "error:  method_end == null" << endl;
      close();
      return ;
    }
    method = string(buf->base, method_end);
  }
  reserve_for_read += string(buf->base, nread);
  if(method == "POST") {
    if(remain == INT_MIN) {
      char* l = strstr(buf->base, "Content-Length:");
      if(!l) {
        cout << "log: " << __LINE__ <<" : " << __FILE__ << " : " 
          << strerror(errno) << endl;
        close();
        return ;
      }
      char* r = strchr(l, '\r');
      if(!r){
        cout << "log: " << __LINE__ <<" : " << __FILE__ << " : " 
          << strerror(errno) << endl;
        close();
        return ;
      }
      string len_s(l + strlen("Content-Length:"), r);
      remain = atoi(len_s.c_str());
      char *d = strstr(buf->base, "\r\n\r\n");
      if(d)
        remain += d - buf->base + 4;
    }
    remain -= nread;
    if(remain <= 0) 
      onRead(this); 
  } else if (method == "GET" || method == "OPTIONS") {
    remain = 0;//means it is not the first time entering the function
    if(reserve_for_read.back() == '\n' && *(reserve_for_read.end() - 2) == '\r'
     && *(reserve_for_read.end() - 3) == '\n' && *(reserve_for_read.end() - 4) == '\r'
    )
    {
      onRead(this);
    } else {
      cout << R"(error in "get" or "options")" << endl;
    }
  }
}