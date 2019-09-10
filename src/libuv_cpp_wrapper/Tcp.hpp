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

class Tcp : public Handle
{
public:
  Tcp(Loop& loop, std::string ip = DEFAULT_IP, int port = DEFAULT_PORT,int backlog = DEFAULT_BACKLOG);
  void addConnection(std::shared_ptr<Connection>&);
  bool listen();
private:
  Loop& loop;
  std::string ip;
  int port;
  int backlog;
  /* uv_handle_t handle(server)
   * which is derived from Handle
   */
  std::vector<std::shared_ptr<Connection>> connectionList;
  static ConnectionCallback connectionCallback;
};
} // namespace uvx
#endif