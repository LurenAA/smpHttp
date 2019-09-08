#ifndef __TCP_ACCEPTOR_H_
#define __TCP_ACCEPTOR_H_

#include "uv.h"
#include <functional>

#define DEFAULT_PORT 8080
#define DEFAULT_IP "127.0.0.1"

namespace net{
  typedef std::function<void(uv_loop_t*, uv_tcp_t*)> ConnectionCallback;
  class TcpAcceptor {
  public:
    TcpAcceptor(uv_loop_t* loop, const char* ip = DEFAULT_IP, int port = DEFAULT_PORT);
    ~TcpAcceptor();

    void listen();
    uv_loop_t* getLoop();
    bool isListen();
    void onNewConnect(uv_tcp_t* client);
    void setNewCallBack(ConnectionCallback callback);
  private:
    bool listened;
    uv_loop_t* loop;
    ConnectionCallback callback;
    uv_tcp_t server;
  };
}
#endif