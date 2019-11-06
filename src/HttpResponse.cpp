#include "HttpResponse.hpp"
#include "Connection.hpp"
#include <iostream>

using namespace std;
using namespace smpHttp;
using namespace uvx;

HttpResponse::HttpResponse(Connection* cl)
 : Packet(), cl(cl)
{
}

void HttpResponse::setAfterWrite(WriteCallback f) {
  cl->setWriteCallback(f);
}

void HttpResponse::end(){
  if(is_end)
    return ;
  // auto old_func = cl->getWriteCallback();
  // auto new_func = bind([](decltype(old_func) f, Connection* cl) {
  //   f();
  //   cl->close();
  // }, old_func, cl);
  // setAfterWrite(new_func);
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
      cout << hrq1->cl->shared_from_this().use_count() << endl;
      if(pfunc) 
        pfunc();
      delete hrq1; 
    }, hrq, pref);
    hrq->cl->setWriteCallback(func);
    cout << hrq->cl->shared_from_this().use_count() << endl;
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