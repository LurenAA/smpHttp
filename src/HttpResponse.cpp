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

void HttpResponse::setAfterWrite(WriteFunc_t f) {
  cl->wfunc = f;
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