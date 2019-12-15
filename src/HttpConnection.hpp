#ifndef _HTTPCONNECTION_HPP
#define _HTTPCONNECTION_HPP
#include "TcpConnection.hpp"
#include "Common.hpp"

namespace xx {
  class HttpConnection : public xx::TcpConnection {
  public:
    using xx::TcpConnection::TcpConnection;
    std::string& getReserveForRead() {return reserve_for_read;}
    
    ssize_t getRemain() const {return remain;}
    void setRemain(ssize_t r) {remain = r;}
    void setMethod(Method m) {method = m;}
    Method getMethod() const {return method;}
  private:
    ssize_t remain = INT_MIN; //  remain len to read
    Method method = Method::NOTSET;
    std::string reserve_for_read = "";
  };
}

#endif //