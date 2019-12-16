#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_
#include "XXLibuv.hpp"
#include <fstream>
#include <map>
#include <functional>
#include <set>
#include "RouteElement.hpp"
#include <vector>
#include <regex>
#include "CCommon.hpp"

namespace xx {
  class HttpResponse;
  class HttpRequest;
  class HttpAccepter : public TcpAccepter {
    using TcpAccepter::TcpAccepter;
    TcpConnection* tcp_connection_object() override;
  };
  class HttpServer{
    public:
      explicit HttpServer(AddressInfo a = AddressInfo{"0.0.0.0", 8080}, 
        EventLoop* lp = new EventLoop(EventLoop::DEFAULT_LOOP));
      ~HttpServer();

      void run();
      void add_route(const std::string& s, routeHandleFunc f,
        Method m = Method::ALL,uint16_t pri = RouteElement::DEFAULT_PRIORITY,
        const std::string& prefix = "", bool is_cover = true);
      void add_static_route(const std::string& s,Method m = Method::ALL,
        uint16_t pri = RouteElement::DEFAULT_PRIORITY , 
        const std::string& prefix = "",bool is_cover = true);
      // void remove_route(const std::string& s);
      
      void add_req(const std::shared_ptr<HttpRequest>&);
      bool remove_req(const std::shared_ptr<HttpRequest>&);
    private:
      EventLoop* _lp;
      HttpAccepter _tcpServer;
      /**
       * 存放路由的回调函数
       **/ 
      std::vector<RouteElement> _route_vec; 
      /**
       * 存放HttpRequest对象的共享指针，
       * 防止在in_read_second之后内存被释放
       **/ 
      std::set<std::shared_ptr<HttpRequest>> _req_set;

      void in_read_second(std::shared_ptr<TcpConnection> tc);
      void in_read(std::shared_ptr<TcpConnection> tc, ssize_t nread, const uv_buf_t *buf, bool isEof);
      void after_connection(std::shared_ptr<TcpConnection> tc);
  };
}
#endif //__HTTPSERVER_H_ 