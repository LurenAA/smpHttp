#ifndef __HTTP_RESPONSE_
#define __HTTP_RESPONSE_
#include "Packet.hpp"
#include "Connection.hpp"

namespace smpHttp {
  using WriteFunc_t = std::function<void()>;
  class HttpResponse : public Packet{
    public:
      void setAfterWrite(WriteFunc_t);
      HttpResponse(uvx::Connection* ); 
      void end();
      ~HttpResponse();
    private:
      bool is_end = false;
      uvx::Connection* cl;
  };
}

#endif //__HTTP_RESPONSE_