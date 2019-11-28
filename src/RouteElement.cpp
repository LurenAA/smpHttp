#include "RouteElement.hpp"
#include <typeinfo>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RouteWq.hpp"

using namespace std;
using namespace smpHttp;

RouteElement::RouteElement(std::regex reg, uint16_t pri, _CbkOrPfx alx,bool is_static, Method m) :
  reg(reg), priority(pri), op(alx), is_static(is_static), method(m)
{
  if(is_static && typeid(alx) == typeid(reinterpret_cast<routeHandleFunc>(0))) 
    op.callback = nullptr;
  else if (!is_static && typeid(alx) == typeid("")) 
    op.prefix = "";
}

bool RouteElement::operator<(const RouteElement& ele) const{
  return this->priority < ele.priority;
}

void RouteElement::static_file_handle(std::shared_ptr<HttpRequest> req
    , std::shared_ptr<HttpResponse> res, const RouteElement& re, std::shared_ptr<RouteWq> rwq)
{

}