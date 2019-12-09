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
struct AddressInfo {
std::string ip;
int port;
};
class TcpAccepter
{
public:
  TcpAccepter(EventLoop&, AddressInfo a = AddressInfo{"0.0.0.0", 8080});
  TcpAccepter(const TcpAccepter&) = delete;
  TcpAccepter& operator=(const TcpAccepter&) = delete;
  virtual ~TcpAccepter() {}

  void listen();

  uv_tcp_t* handle();
  AddressInfo getAddress() const;
  void setAfterConnectionCb(AfterConnectionType);
  void invokeAfterConnectionCb(std::shared_ptr<TcpConnection>);
  // void addConnection(std::shared_ptr<Connection>&);
  // void removeConnection(const std::shared_ptr<Connection>&);
  // ConnectionCallback setConnectionCallback(ConnectionCallback);
  // AfterConnectionCallback setAfterConnectionCallback(AfterConnectionCallback);
  // uv_stream_t* getHandle() {return reinterpret_cast<uv_stream_t *>(handle.get());}
  // Loop& _Loop();
  virtual void on_listen();
  virtual TcpConnection* tcp_connection_object();

  void add_tcp_connection(std::shared_ptr<TcpConnection> c);
  void remove_tcp_connection(int index);
private:
  EventLoop& _lp;  //事件循环
  AddressInfo _address; //地址信息
  uv_tcp_t _server;  //原生handle
  
  // std::shared_ptr<Connection> onConnection(uv_tcp_t*);
  // void onAfterConnection(std::shared_ptr<Connection> c);
  
  // ConnectionCallback connectionCallback = nullptr;
  // AfterConnectionCallback afterConnectionCallback = nullptr;
  AfterConnectionType after_connection_cb;
  std::map<int, std::shared_ptr<TcpConnection>> tcp_connection_map; //to do, close out-of-time connection
  static char* indexes; //用于tcp_connection_map的索引
};
} // namespace uvx
#endif