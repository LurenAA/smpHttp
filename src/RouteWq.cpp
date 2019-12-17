#include "RouteWq.hpp"
#include <cassert>

using namespace std;
using namespace xx;

void RouteWq::next(std::shared_ptr<HttpRequest> req,
        std::shared_ptr<HttpResponse> res) {
  if(wq.empty()) 
    return;
  RouteElement re = wq.top();
  wq.pop();
  re.callback(req, res, *this);
}

void RouteWq::push(const RouteElement& r)
{
  wq.push(r);
}

const RouteElement& RouteWq::top() const {
  return wq.top();
}

void RouteWq::pop() {
  wq.pop();
}