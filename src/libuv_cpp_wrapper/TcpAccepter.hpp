#ifndef __TCP_HPP_
#define __TCP_HPP_
#include "Handle.hpp"
#include "EventLoop.hpp"
#include <string>
#include <functional>
#include <vector>

#define DEFAULT_PORT 8080
#define DEFAULT_IP "0.0.0.0"
#define DEFAULT_BACKLOG 128

/**
 * maybe I`d better use the name TcpServer instead of Tcp
 **/ 
namespace xx {
// using ConnectionCallback = std::function<std::shared_ptr<Connection>(Tcp*, uv_tcp_t*)>;
// using AfterConnectionCallback = std::function<void(std::shared_ptr<Connection> cl)>;
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

  void listen();

  
  // void addConnection(std::shared_ptr<Connection>&);
  // void removeConnection(const std::shared_ptr<Connection>&);
  // ConnectionCallback setConnectionCallback(ConnectionCallback);
  // AfterConnectionCallback setAfterConnectionCallback(AfterConnectionCallback);
  // uv_stream_t* getHandle() {return reinterpret_cast<uv_stream_t *>(handle.get());}
  // Loop& _Loop();
  // void onListen();

private:
  EventLoop& _lp;
  AddressInfo _address;
  uv_tcp_t _server;

  
  // std::shared_ptr<Connection> onConnection(uv_tcp_t*);
  // void onAfterConnection(std::shared_ptr<Connection> c);
  
  // ConnectionCallback connectionCallback = nullptr;
  // AfterConnectionCallback afterConnectionCallback = nullptr;
  // std::vector<std::shared_ptr<Connection>> connectionList; //to do, close out-of-time connection
};
} // namespace uvx
#endif