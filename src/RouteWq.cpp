#include "RouteWq.hpp"
#include <cassert>

using namespace std;
using namespace smpHttp;

RouteWq::RouteWq(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res) :
  req(req), res(res)
{
}

void RouteWq::next() {
  if(wq.empty()) 
    return;
  RouteElement re = wq.top();
  wq.pop();
  if(re.is_static) 
    RouteElement::static_file_handle(req, res, re, shared_from_this());
  else 
    re.op.callback(req, res, re, shared_from_this());
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