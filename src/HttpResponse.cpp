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
  string res = get();
  cl->write(res.c_str(), res.size());
  is_end = true;
}

HttpResponse::~HttpResponse(){
  if(cl->is_active()) 
    end();
}

HttpResponse::HttpResponse(const HttpResponse& s) :
  Packet(), cl(s.cl), is_first(s.is_first)
{
}

void HttpResponseDeleter::operator() (HttpResponse* hrq) const
{
  if(!hrq->is_end) {
    auto messageSize = hrq->getMessageSize();
    
  }
  delete hrq;
}