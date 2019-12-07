#include "TcpAccepter.hpp"
#include <iostream>
#include <algorithm>
#include <cassert>
#include "Tools.hpp"
#include "FileLog.hpp"
#include <exception>
#include "EventLoop.hpp"

using namespace std;
using namespace xx;

TcpAccepter::TcpAccepter(EventLoop& lp, AddressInfo add) :
  _lp(lp), _address(add)
{
  int flag = ::uv_tcp_init(_lp.handle(), &_server);
  if(flag < 0) {
    auto& fl = FileLog::getInstance();
    auto& tl = Tools::getInstance();
    fl.error("uv_tcp_init error : " + tl.get_uv_strerror_t(flag));
    std::terminate();
  }
  _server.data = this;
}

// void Tcp::addConnection(std::shared_ptr<Connection> &newOne)
// {
//   connectionList.push_back(newOne);
// }

// void Tcp::removeConnection(const std::shared_ptr<Connection>& con) {
//   auto deleteOne = find(connectionList.begin(), connectionList.end(), con);
//   if(deleteOne == connectionList.end()) {
//     cerr << "error: con is not in the connectionList" << endl;
//     return ;
//   }
//   connectionList.erase(deleteOne);
// }

// Loop& Tcp::_Loop() {
//   return loop;
// }

// ConnectionCallback Tcp::setConnectionCallback(ConnectionCallback f) {
//   ConnectionCallback pre_fc = connectionCallback;
//   connectionCallback = f;
//   return pre_fc;
// } 

/**
 * 运行listen
 **/ 
void TcpAccepter::listen() {
  struct sockaddr_in addr;
  auto& fl = FileLog::getInstance();
  int flag = uv_ip4_addr(_address.ip.c_str(), _address.port, &addr);
  if (flag < 0)
  {
    auto& tl = Tools::getInstance();
    fl.error("uv_ip4_addr error : " + tl.get_uv_strerror_t(flag));
    terminate();
  }
  flag = uv_tcp_bind(&_server, reinterpret_cast<const sockaddr *>(&addr), 0);
  if (flag < 0)
  {
    auto& tl = Tools::getInstance();
    fl.error("uv_tcp_bind error : " + tl.get_uv_strerror_t(flag));
    terminate();
  }
  flag = ::uv_listen(reinterpret_cast<uv_stream_t*>(&_server), 
    DEFAULT_BACKLOG, [](uv_stream_t *server, int status){
    auto& fl = FileLog::getInstance();  
    if(status < 0) {
      auto& tl = Tools::getInstance();
      fl.error("uv_listen error : " + tl.get_uv_strerror_t(status));
      terminate();
    }
    TcpAccepter *tcp = static_cast<TcpAccepter *>(server->data);
    fl.info("log: listen on port " + tcp->_address.port);
    // tcp->onListen();
  });
  if (flag < 0)
  {
    auto& tl = Tools::getInstance();
    fl.error("uv_listen error : " + tl.get_uv_strerror_t(flag));
    terminate();
  }
}

// void Tcp::onListen(){ //private是针对类的，而不是对象
//   bool error_flag = false;
//   uv_tcp_t *client = new uv_tcp_t();
//   uv_tcp_init(loop.getLoop(), client);
//   int flag2 = uv_accept(getHandle(), reinterpret_cast<uv_stream_t *>(client));
//   if (flag2 < 0)
//   {
//     cerr << "error: uv_accept " << uv_strerror(flag2) << endl;
//     error_flag = true;
//   }
//   else
//   {
//     shared_ptr<Connection> cli = onConnection(client);
//     if(!cli) {
//       cout << "error: onConnection return error" << endl;
//       error_flag = true;
//     } else {
//       addConnection(cli);
//       onAfterConnection(cli);
//     }
//   }
//   if(error_flag){
//     uv_close(reinterpret_cast<uv_handle_t *>(client), [] (uv_handle_t *handle) {
//       delete handle;
//     });
//   }
// }

// std::shared_ptr<Connection> Tcp::onConnection(uv_tcp_t* client) {
//   if(connectionCallback) {
//     return connectionCallback(this, client);
//   } else {
//     return nullptr;
//   }
// }

// AfterConnectionCallback Tcp::setAfterConnectionCallback(AfterConnectionCallback f) {
//   auto pf = afterConnectionCallback;
//   afterConnectionCallback = f;
//   return pf;
// }

// void Tcp::onAfterConnection(std::shared_ptr<Connection> c) {
//   if(afterConnectionCallback)
//     afterConnectionCallback(c);
// }