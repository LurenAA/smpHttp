#ifndef __NET_HPP_
#define __NET_HPP_
#include "Inet.hpp"
#include "radix_tree.h"
#include "http_parser.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include "uv.h"

#define PORT 8080
#define IP "0.0.0.0"
#define DEFAULT_BACKLOG 128

class Net : public Inet {
public: 
  Net(const std::string& ip = IP, int port = PORT, int backlog = DEFAULT_BACKLOG);
  bool add_route(const std::string& path, routeFunc func);
  void change_ip(std::string ipStr);
  void change_port(int);
  void changeBackLog(int); 
  void start_server();
private: 
  std::string ip;  //ip
  int port;  //端口
  int back_log;  //uv_listen的队列长度
  uv_loop_t* def_lp;  //默认事件循环
  uv_tcp_t server;   //默认服务器
public: radix_tree* route_tree;   //路由的基数树
};

#endif