#ifndef __ROUTE_WQ_H_
#define __ROUTE_WQ_H_
#include <queue>
#include "RouteElement.hpp"

namespace xx {
  class HttpResponse;
  class HttpRequest;
  class RouteWq {
    public:
      RouteWq(HttpRequest* req) : req(req) {}
      RouteWq(const RouteWq&) = delete;
      RouteWq& operator=(const RouteWq&) = delete;

      void next(std::shared_ptr<HttpRequest> req,
        std::shared_ptr<HttpResponse> res);
      void push(const RouteElement&);
      void pop();
      const RouteElement& top() const;
    private: 
      HttpRequest* const req;
      std::priority_queue<RouteElement> wq;
  };
}
#endif