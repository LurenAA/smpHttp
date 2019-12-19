#include "RouteElement.hpp"
#include <typeinfo>
#include "RouteWq.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpServer.hpp"
#include "uv.h"
#include "FsWork.hpp"

using namespace std;
using namespace xx;

/**
 * 静态文件操作的回调函数，必须由xx::，不然默认是全局的
 **/ 
void xx::static_file_handle(std::shared_ptr<HttpRequest> req
    , std::shared_ptr<HttpResponse> res, RouteWq& wq)
{
  shared_ptr<FsWork> p = FsWork::newFsWork(req, res, req->getServer());
  string absolute_name = Util::getRootPath() + Util::conbine_prefix_and_path(req->getCurRe()->prefix , req->getRequestPath());
  p->setCb([](Fs* req) {
    FsWork* fs_wk = dynamic_cast<FsWork*>(req);
    shared_ptr<FsWork> wk = fs_wk->shared_from_this();
    
  });
  p->stat(absolute_name);
}

RouteElement::RouteElement(std::regex reg,routeHandleFunc callback,
     bool is_static, std::string prefix, uint16_t pri ,xx::Method m) :
  reg(reg), callback(callback), is_static(is_static), prefix(prefix), priority(pri), method(m)
{
  if (is_static) {
    /**
     * 为什么不推荐形参名字和实参一样，
     * 这里我的意思我想指this->callback
     * 默认却是形参
     **/ 
    this->callback = static_file_handle;
    priority = RouteElement::FILE_PRIORITY;
  }
}

bool RouteElement::operator<(const RouteElement& ele) const{
  return this->priority < ele.priority;
}

