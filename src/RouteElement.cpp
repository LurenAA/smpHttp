#include "RouteElement.hpp"
#include <typeinfo>
#include "RouteWq.hpp"

using namespace std;
using namespace xx;

void static_file_handle(std::shared_ptr<HttpRequest> req
    , std::shared_ptr<HttpResponse> res, std::shared_ptr<RouteWq> rwq)
{

}

RouteElement::RouteElement(std::regex reg, uint16_t pri, std::string prefix,routeHandleFunc callback,
     bool is_static, Method m) :
  reg(reg), priority(pri), prefix(prefix), callback(callback), is_static(is_static), method(m)
{
  if (is_static) 
    callback = static_file_handle;
}

bool RouteElement::operator<(const RouteElement& ele) const{
  return this->priority < ele.priority;
}

