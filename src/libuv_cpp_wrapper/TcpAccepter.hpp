#ifndef __TCP_HPP_
#define __TCP_HPP_
#include "Handle.hpp"
#include "EventLoop.hpp"
#include <string>
#include <functional>
#include <vector>
#include <map>
#include <iterator>
#include "Common.hpp"

#define DEFAULT_PORT 8080
#define DEFAULT_IP "0.0.0.0"
#define DEFAULT_BACKLOG 128

namespace xx {
class TcpConnection;
class TcpAccepter;

class TcpAccepter
{
public:
  explicit TcpAccepter(EventLoop&, AddressInfo a = AddressInfo{"0.0.0.0", 8080});
  TcpAccepter(const TcpAccepter&) = delete;
  TcpAccepter& operator=(const TcpAccepter&) = delete;
  virtual ~TcpAccepter() {}

  void listen();

  uv_tcp_t* handle();
  AddressInfo getAddress() const;
  void setAfterConnectionCb(AfterConnectionType);
  void invokeAfterConnectionCb(std::shared_ptr<TcpConnection>);
 
  virtual void on_listen();
  virtual TcpConnection* tcp_connection_object();
  EventLoop& getLp();

  void add_tcp_connection(std::shared_ptr<TcpConnection> c);
  void remove_tcp_connection(int index);
private:
  EventLoop& _lp;  //事件循环
  AddressInfo _address; //地址信息
  uv_tcp_t _server;  //原生handle
  
  AfterConnectionType after_connection_cb;
  std::map<int, std::shared_ptr<TcpConnection>> tcp_connection_map; //to do, close out-of-time connection
  static char* indexes; //用于tcp_connection_map的索引
};
} // namespace uvx
#endif