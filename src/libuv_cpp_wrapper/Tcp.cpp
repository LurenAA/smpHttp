#include "Tcp.hpp"
#include "Loop.hpp"
#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;
using namespace uvx;

void error_exit_helper(Loop &loop, string place, int flag) noexcept
{
  cerr << "error: " << place << " " << uv_strerror(flag) << endl;
  loop.stop();
  loop.close();
}

Tcp::Tcp(std::string ip, int port, uv_loop_t* l,  int backlog) 
: Handle(new uv_tcp_t()), loop(l), ip(ip), port(port), backlog(backlog)
{
  ::uv_tcp_init(loop.loop, reinterpret_cast<uv_tcp_t *>(handle.get()));
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
  cout << "log: listen on port " << port << endl;
  int flag = uv_listen(reinterpret_cast<uv_stream_t *>(handle.get()), backlog, [](uv_stream_t *server, int status){
    if(status < 0) {
      //to do
      cout << "error: uv_listen error : " << uv_strerror(status) << endl;
    }
    Tcp *tcp = static_cast<Tcp *>(server->data);
    tcp->onListen();
  });
  return flag < 0 ? false : true;
}

void Tcp::addConnection(std::shared_ptr<Connection> &newOne)
{
  connectionList.push_back(newOne);
}

void Tcp::removeConnection(const std::shared_ptr<Connection>& con) {
  auto deleteOne = find(connectionList.begin(), connectionList.end(), con);
  if(deleteOne == connectionList.end()) {
    cerr << "error: con is not in the connectionList" << endl;
    return ;
  }
  connectionList.erase(deleteOne);
}

Loop& Tcp::_Loop() {
  return loop;
}

ConnectionCallback Tcp::setConnectionCallback(ConnectionCallback f) {
  ConnectionCallback pre_fc = connectionCallback;
  connectionCallback = f;
  return pre_fc;
} 

void Tcp::run() {
  listen();
  loop.run();
}

void Tcp::onListen(){ //private是针对类的，而不是对象
  bool error_flag = false;
  uv_tcp_t *client = new uv_tcp_t();
  uv_tcp_init(loop.getLoop(), client);
  int flag2 = uv_accept(getHandle(), reinterpret_cast<uv_stream_t *>(client));
  if (flag2 < 0)
  {
    cerr << "error: uv_accept " << uv_strerror(flag2) << endl;
    error_flag = true;
  }
  else
  {
    Connection *c = onConnection(client);
    if(!c) {
      cout << "error: onConnection return error" << endl;
      error_flag = true;
    } else {
      shared_ptr<Connection> cli(c);
      addConnection(cli);
      onAfterConnection(c);
    }
  }
  if(error_flag){
    uv_close(reinterpret_cast<uv_handle_t *>(client), [] (uv_handle_t *handle) {
      delete handle;
    });
  }
}

Connection* Tcp::onConnection(uv_tcp_t* client) {
  if(connectionCallback) {
    return connectionCallback(this, client);
  } else {
    return nullptr;
  }
}

AfterConnectionCallback Tcp::setAfterConnectionCallback(AfterConnectionCallback f) {
  auto pf = afterConnectionCallback;
  afterConnectionCallback = f;
  return pf;
}

void Tcp::onAfterConnection(Connection* c) {
  if(afterConnectionCallback)
    afterConnectionCallback(c);
}