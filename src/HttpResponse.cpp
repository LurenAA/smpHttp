#include "HttpResponse.hpp"
#include <iostream>

using namespace std;
using namespace smpHttp;
using namespace uvx;

HttpResponse::HttpResponse(Connection* cl)
 : cl(cl)
{
}

void HttpResponse::setAfterWrite(WriteFunc_t f) {
  cl->wfunc = f;
}

void HttpResponse::end(){
  string res = get();
  cl->write(res.c_str(), res.size());
  is_end = true;
}