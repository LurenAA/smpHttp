#include "QueueWork.hpp"
#include "HttpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
using namespace std;
using namespace xx;

QueueWork::QueueWork(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, HttpServer& _server) 
 :req(req), res(res), _server(_server), wk()
{
  wk.data = this;
}

/**
 * 限制创建的对象一定是一个共享指针对象
 **/ 
std::shared_ptr<QueueWork> 
QueueWork::newQueueWork(std::shared_ptr<HttpRequest> req
  , std::shared_ptr<HttpResponse> res,HttpServer& _server) 
{
  /**
   * 这里不能使用make_shared，因为构造函数是私有的
   **/ 
  shared_ptr<QueueWork> rs(new QueueWork(req, res, _server));
  return rs;
}

/**
 * 从HttpServer的_queue_work_set中删除这个对象
 **/ 
bool QueueWork::remove() {
  return _server.remove_queue_work(shared_from_this());
}

/**
 * 注册任务
 **/ 
bool QueueWork::work() {
  auto& fl = FileLog::getInstance();
  /**
   * 添加到server的set中
   **/ 
  _server.add_queue_work(shared_from_this());
  if(!work_cb) {
    fl.debug("QueueWork::work(): no work cb", __func__, __FILE__, __LINE__);
    return false;
  }
  int flag = uv_queue_work(_server.getLoop()->handle(), &wk, 
  [](uv_work_t* wk) {
    QueueWork* wk_p = reinterpret_cast<QueueWork*>(wk->data);
    std::shared_ptr<QueueWork> wk_sp = wk_p->shared_from_this();
    wk_sp->invokeWorkCb();
  }, 
  [](uv_work_t* wk, int status) {
    QueueWork* wk_p = reinterpret_cast<QueueWork*>(wk->data);
    std::shared_ptr<QueueWork> wk_sp = wk_p->shared_from_this();
    wk_sp->invokeAfterWorkCb(status);
    wk_sp->remove();
  });
  if(flag != 0) {
    fl.error("QueueWork::work error : " + Util::get_uv_strerror_t(flag), 
      __func__, __FILE__, __LINE__);
    return false;
  }
  return true;
}

void QueueWork::invokeWorkCb() {
  if(this->work_cb)
    work_cb(shared_from_this());
}

void QueueWork::invokeAfterWorkCb(int sta) {
  if(this->after_work_cb) 
    after_work_cb(shared_from_this(), sta);
}

std::shared_ptr<HttpRequest> QueueWork::getReq() const {
  return req;
}

std::shared_ptr<HttpResponse> QueueWork::getRes() const {
  return res;
}

HttpServer& QueueWork::getServer() const {
  return _server;
}
