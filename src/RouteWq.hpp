#ifndef __ROUTE_WQ_H_
#define __ROUTE_WQ_H_
#include <queue>
#include "RouteElement.hpp"

namespace smpHttp {
  class HttpResponse;
  class HttpRequest;
  class RouteWq : public std::enable_shared_from_this<RouteWq>{
    public:
      RouteWq(std::shared_ptr<HttpRequest> req,
      std::shared_ptr<HttpResponse> res); 
      RouteWq(const RouteWq&) = delete;
      RouteWq& operator=(const RouteWq&) = delete;

      void next();
      void push(const RouteElement&);
      void pop();
      const RouteElement& top() const;
    private: 
      std::shared_ptr<HttpRequest> req;
      std::shared_ptr<HttpResponse> res;
      std::priority_queue<RouteElement> wq;
  };
}
#endif