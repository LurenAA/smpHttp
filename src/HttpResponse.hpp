#ifndef __HTTP_RESPONSE_
#define __HTTP_RESPONSE_
#include "Packet.hpp"
#include <functional>

namespace uvx {
  class Connection;
}

namespace smpHttp {
  using WriteCallback = std::function<void()>;
  class HttpResponse : public Packet{
    public:
      HttpResponse() = default;
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
}

#endif //__HTTP_RESPONSE_