#include "Tcp.hpp"
#include "Loop.hpp"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace uvx;

ConnectionCallback Tcp::connectionCallback = nullptr;

void error_exit_helper(Loop &loop, string place, int flag)
{
  cerr << "error: " << place << " " << uv_strerror(flag) << endl;
  loop.stop();
  loop.close();
  exit(1);
}

Tcp::Tcp(Loop &loop, std::string ip, int port, int backlog) : Handle(new uv_tcp_t()), loop(loop), ip(ip), port(port), backlog(backlog)
{
  uv_tcp_init(loop.loop, reinterpret_cast<uv_tcp_t *>(handle.get()));
  struct sockaddr_in addr;
  int flag = uv_ip4_addr(ip.c_str(), port, &addr);
  if (flag < 0)
  {
    error_exit_helper(loop, "uv_ip4_addr", flag);
    return;
  }
  flag = uv_tcp_bind(reinterpret_cast<uv_tcp_t *>(handle.get()),
                     reinterpret_cast<const sockaddr *>(&addr), 0);
  if (flag < 0)
  {
    error_exit_helper(loop, "uv_tcp_bind", flag);
    return;
  }
}

bool Tcp::listen()
{
  // if(!Tcp::connectionCallback){
  //   cerr << "error: tcp needs a connectionCallback" << endl;
  //   return false;
  // }
  cout << "log: listen on port " << port << endl;
  int flag = uv_listen(reinterpret_cast<uv_stream_t *>(handle.get()), backlog, uvx::listenHandle);
  return flag < 0 ? false : true;
}

void Tcp::addConnection(std::shared_ptr<Connection> &newOne)
{
  connectionList.push_back(newOne);
}

/**
 * handle coming connections, and make a Connection Object,
 * then put it into the connectionList
 **/
void uvx::listenHandle(uv_stream_t *server, int status)
{
  Tcp *tcp = static_cast<Tcp *>(server->data);
  uv_tcp_t *client = new uv_tcp_t();
  uv_tcp_init(tcp->loop.getLoop(), client);
  int flag2 = uv_accept(reinterpret_cast<uv_stream_t *>(tcp->handle.get()),
                        reinterpret_cast<uv_stream_t *>(client));
  if (flag2 < 0)
  {
    cerr << "error: uv_accept " << uv_strerror(flag2) << endl;
    uv_close(reinterpret_cast<uv_handle_t *>(client), [] (uv_handle_t *handle) {
      delete handle;
    });
    delete client;
  }
  else
  {
    Connection *c = new Connection(client, tcp);
    shared_ptr<Connection> cli(c);
    tcp->addConnection(cli);
    if (Tcp::connectionCallback)
      Tcp::connectionCallback(reinterpret_cast<uv_stream_t *>(tcp->handle.get()), client);
  }
}

void Tcp::removeConnection(const std::shared_ptr<Connection>& con) {
  auto deleteOne = find(connectionList.begin(), connectionList.end(), con);
  if(deleteOne == connectionList.end()) {
    cerr << "error: con is not in the connectionList" << endl;
    return ;
  }
  connectionList.erase(deleteOne);
}