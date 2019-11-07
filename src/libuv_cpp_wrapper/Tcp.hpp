#ifndef __TCP_HPP_
#define __TCP_HPP_
#include "Handle.hpp"
#include "Connection.hpp"
#include "Loop.hpp"
#include <string>
#include <functional>
#include <vector>

#define DEFAULT_PORT 8080
#define DEFAULT_IP "0.0.0.0"
#define DEFAULT_BACKLOG 128

/**
 * maybe I`d better use the name TcpServer instead of Tcp
 **/ 
namespace uvx {
using ConnectionCallback = std::function<std::shared_ptr<Connection>(Tcp*, uv_tcp_t*)>;
using AfterConnectionCallback = std::function<void(std::shared_ptr<Connection> cl)>;
class Loop;
class Tcp : public Handle
{
public:
  Tcp(std::string ip = DEFAULT_IP, int port = DEFAULT_PORT,uv_loop_t* l = uv_default_loop(), int backlog = DEFAULT_BACKLOG);
  Tcp(const Tcp&) = delete;
  Tcp& operator=(const Tcp&) = delete;
  void run();

  void addConnection(std::shared_ptr<Connection>&);
  void removeConnection(const std::shared_ptr<Connection>&);
  ConnectionCallback setConnectionCallback(ConnectionCallback);
  AfterConnectionCallback setAfterConnectionCallback(AfterConnectionCallback);
  uv_stream_t* getHandle() {return reinterpret_cast<uv_stream_t *>(handle.get());}
  Loop& _Loop();
  void onListen();

private:
  Loop loop;
  std::string ip;
  int port;
  int backlog;
  /* uv_handle_t handle(server)
   * which is derived from Handle
   */
  bool listen();
  std::shared_ptr<Connection> onConnection(uv_tcp_t*);
  void onAfterConnection(std::shared_ptr<Connection> c);
  
  ConnectionCallback connectionCallback = nullptr;
  AfterConnectionCallback afterConnectionCallback = nullptr;
  std::vector<std::shared_ptr<Connection>> connectionList; //to do, close out-of-time connection
};
} // namespace uvx
#endif