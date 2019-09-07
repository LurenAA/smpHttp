#include "Net.hpp"
#include "http_request_handles.hpp"
#include "uv_handles.hpp"

#define CHECK(e) if(e < 0) {\
  fprintf(stderr, "%s: %s\n",uv_err_name(e), uv_strerror(e)); \
  exit(1);}

extern
http_parser_settings settings;  //http_parser的配置

/**
 * 构造函数
 **/ 
Net::Net(const std::string& ip , int port, int backlog) 
  : ip(ip), port(port), back_log(backlog), def_lp(uv_default_loop()), route_tree(radixNewTree())
{
  if(!route_tree) {
    fprintf(stderr, "error: init route_tree error in Net");
    exit(EXIT_FAILURE);
  }
  server.data = this;
}

/**
 * 改变IP值
 **/ 
void Net::change_ip(std::string ipStr) {
  ip = ipStr;
}

/**
 * 改变端口号
 **/ 
void Net::change_port(int aport) {
  port = aport;
}

/**
 * 改变listen时backlog的值
 **/ 
void Net::changeBackLog(int aback_log) {
  back_log = aback_log;
}

/**
 * 添加路由
 **/ 
bool Net::add_route(const std::string& path, routeFunc func) {
  return radixInsert(route_tree, path.c_str(), path.size(), (void*)func);
}

/**
 * 运行服务器
 **/
void Net::start_server() {
  init_http_settings(&settings);
  int r; //just like errno
  r = uv_tcp_init(def_lp, &server);
  CHECK(r);
  struct sockaddr_in sockAddr;
  r = uv_ip4_addr(ip.c_str(), port, &sockAddr);
  CHECK(r);
  r = uv_tcp_bind(&server, (struct sockaddr *)&sockAddr, 0);
  CHECK(r);
  r = uv_listen((uv_stream_t *)&server, back_log, uv_connection_cb_func);
  CHECK(r);
  fprintf(stdout, "listen in %d\n", port);
  uv_run(def_lp, UV_RUN_DEFAULT);
}

