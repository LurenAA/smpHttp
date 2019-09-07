#include "RequestContext.hpp"
#include "HttpRequest.hpp"
#include <iostream>

RequestContext::RequestContext() :
  req(new HttpRequest()), end_signal(false)
{
  http_parser_init(&parser, HTTP_REQUEST);
  parser.data = this;
  write_req = (uv_write_t* )malloc(sizeof(uv_write_t));
  if(!write_req) {
    std::cout << "error: in RequestContext malloc" << std::endl;
  }
  write_req->data = this;
  tcp = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  if(!tcp) {
    std::cout << "error: in RequestContext malloc" << std::endl;
  }
  tcp->data = this;
  uv_tcp_init(uv_default_loop(), tcp);
}

RequestContext::~RequestContext() {
  free(tcp);
  free(write_req);
  delete req;
}