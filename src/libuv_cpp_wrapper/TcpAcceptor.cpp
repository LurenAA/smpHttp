#include "TcpAcceptor.hpp"
#include <iostream>

using namespace std;
using namespace net;

TcpAcceptor::TcpAcceptor(uv_loop_t *loop, const char *ip, int port)
    : loop(loop), callback(nullptr)
{
  struct sockaddr_in addr;
  ::uv_ip4_addr(ip, port, &addr);
  ::uv_tcp_init(loop, &server);
  ::uv_tcp_bind(&server, (const sockaddr *)&addr, 0);
  server.data = (void *)this;
}

TcpAcceptor::~TcpAcceptor()
{
}

void TcpAcceptor::listen()
{
  ::uv_listen((uv_stream_t *)&server, 128,
              [](uv_stream_t *server, int status) {
                if (status < 0)
                {
                  cout << "new connection error:" << uv_strerror(status) << endl;
                  return;
                }
                TcpAcceptor *accept = (TcpAcceptor *)(server->data);
                uv_tcp_t *client = new uv_tcp_t();
                uv_tcp_init(accept->getLoop(), client);
                if (::uv_accept(server, (uv_stream_t *)client))
                {
                  accept->onNewConnect(client);
                }
                else
                {
                  ::uv_close((uv_handle_t *)client, NULL);
                  delete client;
                }
              });
  listened = true;
}

uv_loop_t *TcpAcceptor::getLoop()
{
  return this->loop;
}

void TcpAcceptor::onNewConnect(uv_tcp_t *client)
{
  if (nullptr != callback)
  {
    callback(loop, client);
  }
}

bool TcpAcceptor::isListen()
{
  return listened;
}

void TcpAcceptor::setNewCallBack(ConnectionCallback callback)
{
  this->callback = callback;
}