#include "HttpResponse.hpp"
#include "Connection.hpp"
#include <iostream>

using namespace std;
using namespace smpHttp;
using namespace uvx;

HttpResponse::HttpResponse(std::shared_ptr<uvx::Connection> cl)
 : Packet(), cl(cl)
{
}

void HttpResponse::setAfterWrite(WriteCallback f) {
  cl->setWriteCallback(f);
}

void HttpResponse::end(){
  if(is_end)
    return ;
  cl->write(get());
  is_end = true;
}

HttpResponse::~HttpResponse(){
  
}

HttpResponse::HttpResponse(const HttpResponse& s) :
  Packet(), cl(s.cl), is_first(s.is_first)
{
}

void HttpResponseDeleter::operator() (HttpResponse* hrq) const
{
  if(!hrq->is_end) { 
    auto pref = hrq->cl->getWriteCallback();
    auto func = bind([](HttpResponse* hrq1, decltype(pref) pfunc) {
      if(pfunc) 
        pfunc();
      delete hrq1; 
    }, hrq, pref);
    hrq->cl->setWriteCallback(func);
    if(hrq->cl->is_active()) 
      hrq->end();
  } else {
    auto pref = hrq->cl->getWriteCallback();
    if(pref)
      pref();
    delete hrq;
  }
}

void HttpResponse::close() {
  cl->close();
}