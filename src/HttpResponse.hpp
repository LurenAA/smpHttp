#ifndef __HTTP_RESPONSE_
#define __HTTP_RESPONSE_
#include "Packet.hpp"
#include <functional>
#include <iostream>
#include <memory>

namespace uvx {
  class Connection;
}

namespace smpHttp {
  using WriteCallback = std::function<void()>;
  class HttpResponse : public Packet{
    public:
      HttpResponse() = delete;
      HttpResponse(const HttpResponse&);
      void setAfterWrite(WriteCallback);
      HttpResponse(uvx::Connection* ); 
      void end();
      ~HttpResponse() override;
      void setNotFirst() {is_first = false;}
      bool isFirst() {return is_first;}
    private:
      bool is_end = false;
      uvx::Connection* cl;
      bool is_first = true;  //if it is firstly enter the callback
  };
  struct HttpResponseDeleter {
    void operator() (HttpResponse* hrq) const 
    {
      std::cout << "before delete" << std::endl;
      delete hrq;
      std::cout << "end delete" << std::endl;
    }
  };
  template<typename... A>
  std::shared_ptr<HttpResponse> newHttpResponse(A... args) {
    return std::shared_ptr<HttpResponse> (args..., HttpResponseDeleter());
  }
}

#endif //__HTTP_RESPONSE_