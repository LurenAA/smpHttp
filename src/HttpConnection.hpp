#ifndef _HTTPCONNECTION_HPP
#define _HTTPCONNECTION_HPP
#include "Connection.hpp"

namespace smpHttp {
  class HttpConnection : public uvx::Connection {
  public:
    using uvx::Connection::Connection;
    const std::string& getDatagram() {return reserve_for_read;}
  private:
    ssize_t remain = INT_MIN; //  remain len to read
    std::string method = "";
    std::string reserve_for_read = "";
    void onStartRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) override;
  };
}

#endif //