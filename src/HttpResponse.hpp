#ifndef __HTTP_RESPONSE_
#define __HTTP_RESPONSE_
#include "Packet.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include "TcpConnection.hpp"
#include "CCommon.hpp"

namespace xx {
  class HttpResponse;
  /**
   * 在HttpResponse删除的时候做一些处理
   **/ 
  struct HttpResponseDeleter { //必须放在这里，不然是imcomplete type，无法声明operator（）为友元
    void operator() (HttpResponse* hrq) const;
  };
  class HttpResponse : public Packet{
    friend void HttpResponseDeleter::operator() (HttpResponse* ) const; //给删除器友元
    public:
      HttpResponse(const HttpResponse&) = delete;
      HttpResponse& operator=(const HttpResponse&) = delete;
      HttpResponse(std::shared_ptr<TcpConnection>); 
      ~HttpResponse() override {};

      std::shared_ptr<HttpResponse> getStatuCopy() const;

      void setAfterWrite(AfterWriteType);
      bool end();
      
      void setNotFirst() {is_first = false;}
      bool getIsFirst() const {return is_first;}
      void close(); 
      bool getIsEnd() const {return is_end;}
    private:
      bool is_end = false; //是否已经发送过
      std::shared_ptr<TcpConnection> cl;
      /**
       * is_first 主要影响的是chunked模式下的传输，
       * chunked模式下第一次传输和后面的包格式是不一样的
       **/ 
      bool is_first = true;  
  };
  /**
   *工厂模式的一种实现
   **/ 
  template<typename... A>
  std::shared_ptr<HttpResponse> newHttpResponse(A... args) {
    return std::shared_ptr<HttpResponse> (new HttpResponse(args...), HttpResponseDeleter());
  }
}

#endif //__HTTP_RESPONSE_