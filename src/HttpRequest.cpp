#include <iostream>
#include "HttpRequest.hpp"
#include "FileLog.hpp"
#include <functional>
#include "HttpServer.hpp"

using namespace std;
using namespace xx;

/**
 * 关闭连接
 **/ 
void HttpRequest::close() {
  auto& fl = FileLog::getInstance();
  fl.debug("close HttpRequest " 
    + connection->getIndex(), __func__, __FILE__, __LINE__);
  // /**
  //  * 设置关闭时的回调，从server中移除req
  //  **/ 
  // auto close_cb1 =bind([](std::shared_ptr<HttpRequest> rq, Handle* hd){
  //   HttpServer& svr = rq->getServer();
  //   svr.remove_req(rq);
  // } , shared_from_this(), std::placeholders::_1);
  // connection->setInCloseCb(close_cb1);
  connection->close();
}

HttpRequest::HttpRequest(const xx::HttpResult& s, std::shared_ptr<xx::TcpConnection> c,xx::HttpServer& lp)
  : HttpResult(s), connection(c), svr(lp), _route(this) 
{
  // svr.add_req(shared_from_this()); 不能再构造函数中使用,这时还不是一个shared_ptr指针
}

/**
 * 构造函数，限定只能由共享指针初始化
 **/ 
std::shared_ptr<HttpRequest> 
HttpRequest::newHttpRequest(const xx::HttpResult& s, std::shared_ptr<xx::TcpConnection> c,xx::HttpServer& svr)
{
  std::shared_ptr<HttpRequest> sr(new HttpRequest(s, c, svr));
  // svr.add_req(sr);
  return sr;
}

/**
 * 返回HttpServer对象的引用
 **/ 
HttpServer& HttpRequest::getServer () const {
  return svr;
}

void HttpRequest::setCurRe(const RouteElement& el){
  this->cur_re = el;
}

const RouteElement& HttpRequest::getCurRe() const {
  return this->cur_re;
}

std::shared_ptr<TcpConnection> HttpRequest::getTcpConnection()
{
  return connection;
}