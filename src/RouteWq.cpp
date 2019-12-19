#include "RouteWq.hpp"
#include <cassert>
#include "FileLog.hpp"
#include "HttpRequest.hpp"

using namespace std;
using namespace xx;

void RouteWq::next(std::shared_ptr<HttpRequest> req,
        std::shared_ptr<HttpResponse> res) {
  try {
    if(wq.empty()) 
      return;
    RouteElement re = wq.top();
    wq.pop();
    if(re.callback) {
      req->setCurRe(&re);
      re.callback(req, res, *this);
      req->setCurRe(nullptr);
    }
  } catch(exception& e) {
    FileLog::getInstance().error(string("RouteWq::next") + e.what(), __func__, __FILE__, __LINE__);
  }
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