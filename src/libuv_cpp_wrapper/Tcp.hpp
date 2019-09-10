#ifndef __TCP_HPP_
#define __TCP_HPP_
#include "Handle.hpp"
#include "Connection.hpp"
#include <string>
#include <functional>
#include <vector>

#define DEFAULT_PORT 8080
#define DEFAULT_IP "0.0.0.0"
#define DEFAULT_BACKLOG 128

namespace uvx {
using ConnectionCallback = std::function<void(uv_stream_t* server, uv_tcp_t* tcp)>;
class Loop;
void listenHandle(uv_stream_t *server, int status);

class Tcp : public Handle
{
friend void uvx::listenHandle(uv_stream_t *server, int status);
friend class Connection;
public:
  Tcp(Loop& loop, std::string ip = DEFAULT_IP, int port = DEFAULT_PORT,int backlog = DEFAULT_BACKLOG);
  bool listen();
private:
  void addConnection(std::shared_ptr<Connection>&);
  void removeConnection(std::shared_ptr<Connection>&);
  Loop& loop;
  std::string ip;
  int port;
  int backlog;
  /* uv_handle_t handle(server)
   * which is derived from Handle
   */
  std::vector<std::shared_ptr<Connection>> connectionList; //to do, close out-of-time connection
  static ConnectionCallback connectionCallback;
};
} // namespace uvx
#endif