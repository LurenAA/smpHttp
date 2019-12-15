#ifndef __HTTP_RESPONSE_
#define __HTTP_RESPONSE_
#include "Packet.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <TcpConnection.hpp>

namespace smpHttp {
  using WriteCallback = std::function<void()>;
  class HttpResponse;
  struct HttpResponseDeleter { //必须放在这里，不然是imcomplete type，无法声明operator（）为友元
    void operator() (HttpResponse* hrq) const;
  };
  class HttpResponse : public Packet{
    friend void HttpResponseDeleter::operator() (HttpResponse* ) const; //给删除器友元
    public:
      HttpResponse() = delete;
      HttpResponse(const HttpResponse&);
      void setAfterWrite(WriteCallback);
      HttpResponse(std::shared_ptr<uvx::Connection>); 
      void end();
      ~HttpResponse() override;
      void setNotFirst() {is_first = false;}
      bool isFirst() {return is_first;}
      void close(); 
      bool isEnd() {return is_end;}
    private:
      bool is_end = false; //是否已经发送过
      std::shared_ptr<uvx::Connection> cl;
      bool is_first = true;  //是否是第一次进入回调函数
  };
  template<typename... A>
  std::shared_ptr<HttpResponse> newHttpResponse(A... args) {
    return std::shared_ptr<HttpResponse> (new HttpResponse(args...), HttpResponseDeleter());
  }
}

#endif //__HTTP_RESPONSE_