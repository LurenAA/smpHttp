#ifndef __REQUSET_CONTEXT_HPP
#define __REQUSET_CONTEXT_HPP
#include "uv.h"
#include "http_parser.h"
struct HttpRequest;

struct RequestContext {
  RequestContext(); 
  ~RequestContext();
  uv_tcp_t* tcp;
  http_parser parser;
  uv_write_t* write_req;
  HttpRequest* req;
  bool end_signal;
};

#endif