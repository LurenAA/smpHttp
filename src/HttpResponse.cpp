#include "HttpResponse.hpp"
#include <iostream>
#include "FileLog.hpp"
#include "HttpRequest.hpp"


using namespace std;
using namespace xx;

/**
 * 通过一个TcpConnection的对象来初始化，
 * 用在HttpServer的in_read_second
 **/ 
HttpResponse::HttpResponse(std::shared_ptr<TcpConnection> cl, std::shared_ptr<HttpRequest> hrq)
 : Packet(), cl(cl), req(hrq)
{
}

/**
 * 设置完成write操作后的回调函数
 **/ 
void HttpResponse::setAfterWrite(AfterWriteType f) {
  cl->setAfterWriteCb(f);
}

/**
 * 显示调动end，发送数据包。
 * 如果写过那么返回false
 **/ 
bool HttpResponse::end(){
  if(is_end || cl->is_closing() || !cl->is_active())
    return false;
  cl->write(get());
  is_end = true;
  return true;
}

void HttpResponseDeleter::operator() (HttpResponse* res) const
{
  /**
   *如果是普通请求并且在最后没有发送数据报
   **/ 
  if(res->getMode() == TransMode::NORMAL && !res->getIsEnd()) {
    /**
     * 如果用户没有自己设置回调函数
     **/ 
    if(!res->getAfterWriteCb()) {
      res->setAfterWrite([](std::shared_ptr<TcpConnection> tc) {
        tc->close();
      });
    }
    res->end(); 
  } else if (res->getMode() == TransMode::CHUNKED && !res->getIsEnd()) {
    res->end();
  } else if (res->getMode() == TransMode::CHUNKED_START  && !res->getIsEnd()) {
    res->end();
  }
  delete res;
  
  // if(!hrq->is_end) { 
  //   auto pref = hrq->cl->getWriteCallback();
  //   auto func = bind([](HttpResponse* hrq1, decltype(pref) pfunc) {
  //     if(pfunc) 
  //       pfunc();
  //     delete hrq1; 
  //   }, hrq, pref);
  //   hrq->cl->setWriteCallback(func);
  //   if(hrq->cl->is_active()) 
  //     hrq->end();
  // } else {
  //   auto pref = hrq->cl->getWriteCallback();
  //   if(pref)
  //     pref();
  //   delete hrq;
  // }
}

/**
 * 关闭连接
 **/ 
void HttpResponse::close() {
  req->close();
}
