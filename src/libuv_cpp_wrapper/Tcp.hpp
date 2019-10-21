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
using ConnectionCallback_d = std::function<void(Tcp*, Connection*)>;
using ConnectionCallback = std::function<void(Connection*)>;
using ListenCallback = std::function<void()>;
using ListenCallback_d = std::function<void(Tcp*)>;
class Loop;
void listenHandle(uv_stream_t *server, int status);

class Tcp : public Handle
{
friend void Connection::close_cb();
public:
  Tcp(uv_loop_t* l = uv_default_loop(),std::string ip = DEFAULT_IP, int port = DEFAULT_PORT,int backlog = DEFAULT_BACKLOG);
  Tcp(const Tcp&) = delete;
  Tcp& operator=(const Tcp&) = delete;
  void run();

protected:  
  Loop loop;

  uv_loop_t * getLoop(); 
  uv_stream_t* getHandle() {return reinterpret_cast<uv_stream_t *>(handle.get());}
  void addConnection(std::shared_ptr<Connection>&);
  void removeConnection(const std::shared_ptr<Connection>&);
  ConnectionCallback setConnectionCallback(ConnectionCallback_d);
  ListenCallback setListenCallback(ListenCallback_d);
  void callConnectionCallback(uvx::Connection *c) { theConnectionCallback(c); }

private:
  std::string ip;
  int port;
  int backlog;
  /* uv_handle_t handle(server)
   * which is derived from Handle
   */
  bool listen();

  ConnectionCallback connectionCallback = nullptr;
  ListenCallback listenCallback = nullptr;
  virtual void theConnectionCallback(Connection*)  = 0; // ,to change into shared_ptr
  virtual void theListenCallback() = 0; // = 0
  std::vector<std::shared_ptr<Connection>> connectionList; //to do, close out-of-time connection
};
} // namespace uvx
#endif