#include "TcpAccepter.hpp"
#include <iostream>
#include <algorithm>
#include <cassert>
#include "Tools.hpp"
#include "FileLog.hpp"
#include <exception>
#include "EventLoop.hpp"
#include "TcpConnection.hpp"
#include <algorithm>

using namespace std;
using namespace xx;

TcpAccepter::TcpAccepter(EventLoop& lp, AddressInfo add) :
  _lp(lp), _address(add)
{
  int flag = ::uv_tcp_init(_lp.handle(), &_server);
  if(flag < 0) {
    auto& fl = FileLog::getInstance();
    auto& tl = Tools::getInstance();
    fl.error("uv_tcp_init error : " + tl.get_uv_strerror_t(flag), __func__, __FILE__, __LINE__);
    std::terminate();
  }
  _server.data = this;
}

EventLoop& TcpAccepter::getLp() {
  return _lp;
}

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
    fl.error("uv_ip4_addr error : " + tl.get_uv_strerror_t(flag), __func__, __FILE__, __LINE__);
    terminate();
  }
  flag = uv_tcp_bind(&_server, reinterpret_cast<const sockaddr *>(&addr), 0);
  if (flag < 0)
  {
    auto& tl = Tools::getInstance();
    fl.error("uv_tcp_bind error : " + tl.get_uv_strerror_t(flag), __func__, __FILE__, __LINE__);
    terminate();
  }
  fl.info("listen on port " + to_string(getAddress().port), __func__, __FILE__, __LINE__);
  flag = ::uv_listen(reinterpret_cast<uv_stream_t*>(&_server), 
    DEFAULT_BACKLOG, [](uv_stream_t *server, int status){
    auto& fl = FileLog::getInstance();  
    if(status < 0) {
      auto& tl = Tools::getInstance();
      fl.error("uv_listen error : " + tl.get_uv_strerror_t(status), __func__, __FILE__, __LINE__);
      terminate();
    }
    TcpAccepter *tcp = static_cast<TcpAccepter *>(server->data);
    tcp->on_listen();
  });
  if (flag < 0)
  {
    auto& tl = Tools::getInstance();
    fl.error("uv_listen error : " + tl.get_uv_strerror_t(flag), __func__, __FILE__, __LINE__);
    terminate();
  }
}

/**
 * 接受请求
 **/ 
void TcpAccepter::on_listen(){ 
  int flag2;
  shared_ptr<TcpConnection> tp(tcp_connection_object());
  if(!tp.get()) {
    auto& fl = FileLog::getInstance();
    fl.debug("TcpAccepter -> tcp_connection_object error : " 
    , __func__, __FILE__, __LINE__);
    goto Error;
  }
  flag2 = ::uv_accept(reinterpret_cast<uv_stream_t *>(handle()), 
    reinterpret_cast<uv_stream_t *>(tp->handle()));
  if (flag2 < 0)
  {
    auto& tl = Tools::getInstance();
    auto& fl = FileLog::getInstance();
    fl.debug("TcpAccepter -> on_listen error : " + tl.get_uv_strerror_t(flag2)
      , __func__, __FILE__, __LINE__);
    goto Error;
  }
  else
  {
    add_tcp_connection(tp);
    invokeAfterConnectionCb(tp);
  }
  return ;
Error: 
  tp->close();
}

/**
 * 获得地址信息
 **/ 
AddressInfo TcpAccepter::getAddress() const {
  return _address;
}

/**
 * 原生接口
 **/ 
uv_tcp_t* TcpAccepter::handle() {
  return &_server;
}

/**
 * 设置连接对象的类型，可能是TcpConnection的子类
 **/ 
TcpConnection* TcpAccepter::tcp_connection_object() {
  return new TcpConnection(*this, _lp);
}

char* TcpAccepter::indexes = new char[INT_MAX / 100]();
/**
 * 添加连接到map中
 **/ 
void TcpAccepter::add_tcp_connection(std::shared_ptr<TcpConnection> c) {
  auto& fl = FileLog::getInstance();
  auto rs_fd = find_if(TcpAccepter::indexes, TcpAccepter::indexes + INT_MAX / 100, [](const char& val) {
    return val == 0;
  });
  if(rs_fd == indexes + INT_MAX / 100) {
    fl.error("tcp connection exceed the limit", __func__, __FILE__, __LINE__);
    terminate();
  }
  int index = rs_fd - indexes;
  indexes[index] = 1;
  tcp_connection_map.insert({index, c});
  c->setIndex(index);
  fl.info("add a connection : " + to_string(index), __func__, __FILE__, __LINE__);
}

/**
 * 除掉一个链接
 **/ 
void TcpAccepter::remove_tcp_connection(int index) {
  auto& fl = FileLog::getInstance();
  auto fx = tcp_connection_map.find(index);
  if(fx == tcp_connection_map.end()) {
    fl.debug("TcpAccepter -> remove_tcp_connection: no such index", __func__, __FILE__, __LINE__);
    return ;
  }
  assert((fx->second->getIndex() == fx->first));
  indexes[fx->first] = 0;
  fl.info("remove a connection : " + to_string(fx->first), __func__, __FILE__, __LINE__);
  tcp_connection_map.erase(fx);
}


/**
 * 设置连接后的回调函数
 **/ 
void TcpAccepter::setAfterConnectionCb(AfterConnectionType f) {
  after_connection_cb = f;
}

/**
 * 调用连接后的回调函数
 **/ 
void TcpAccepter::invokeAfterConnectionCb(std::shared_ptr<TcpConnection> tp) {
  if(after_connection_cb)
    after_connection_cb(tp);
}