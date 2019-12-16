#include "RouteElement.hpp"
#include <typeinfo>
#include "RouteWq.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

using namespace std;
using namespace xx;

void static_file_handle(std::shared_ptr<HttpRequest> req
    , std::shared_ptr<HttpResponse> res, std::shared_ptr<RouteWq> rwq)
{

}

RouteElement::RouteElement(std::regex reg,routeHandleFunc callback,
     bool is_static, std::string prefix, uint16_t pri ,xx::Method m) :
  reg(reg), callback(callback), is_static(is_static), prefix(prefix), priority(pri), method(m)
{
  if (is_static) 
    callback = static_file_handle;
}

bool RouteElement::operator<(const RouteElement& ele) const{
  return this->priority < ele.priority;
}

