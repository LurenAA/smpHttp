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
  class QueueWork;
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
      EventLoop* getLoop() const {return _lp;}
      void add_route(const std::string& s, routeHandleFunc f,
        Method m = Method::ALL,uint16_t pri = RouteElement::DEFAULT_PRIORITY,
        const std::string& prefix = "", bool is_cover = true);
      void add_static_route(const std::string& s,Method m = Method::ALL,
        uint16_t pri = RouteElement::FILE_PRIORITY, 
        const std::string& prefix = "",bool is_cover = true);
      // void remove_route(const std::string& s);
      
      void add_queue_work(std::shared_ptr<QueueWork>);
      bool remove_queue_work(std::shared_ptr<QueueWork>);
    private:
      EventLoop* _lp;
      HttpAccepter _tcpServer;
      /**
       * 存放路由的回调函数
       **/ 
      std::vector<RouteElement> _route_vec; 
      /**
       * 当注册的函数中存在异步操作时，使用这个set来保持
       * HttpRequest和HttpResponse的生命周期
       **/ 
      std::set<std::shared_ptr<QueueWork>> _queue_work_set;

      void in_read_second(std::shared_ptr<TcpConnection> tc);
      void in_read(std::shared_ptr<TcpConnection> tc, ssize_t nread, const uv_buf_t *buf, bool isEof);
      void after_connection(std::shared_ptr<TcpConnection> tc);
  };
}
#endif //__HTTPSERVER_H_ 