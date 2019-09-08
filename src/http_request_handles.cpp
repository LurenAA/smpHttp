#include "http_request_handles.hpp"
#include "RequestContext.hpp"
#include "uv_handles.hpp"
#include "HttpRequest.hpp"
#include "radix_tree.h"
#include "Net.hpp"
#include <fstream>
#include <iostream>
#include <string>

typedef std::string (*routeFunc)(RequestContext*);

#define RESPONSE \
"HTTP/1.1 404 NOT FOUND\r\n" \
"Content-Type: text/plain\r\n" \
"Content-Length: 9\r\n" \
"\r\n" \
"Not Found"

#define CHUNKED_HEAD \
"HTTP/1.1 200 OK\r\n" \
"Content-Type: text/html\r\n" \
"Connection:keep-alive\r\n" \
"Transfer-Encoding: chunked\r\n\r\n" \

uv_buf_t sbuf;

int on_message_begin_func(http_parser * p) {
  return 0;
}

int on_header_value_func(http_parser * p, const char *at, size_t length){

  return 0;
}

int on_header_field_func(http_parser * p, const char *at, size_t length){

  return 0;
}

int on_status_func(http_parser * p, const char *at, size_t length){

  return 0;
}

int on_url_func(http_parser * p, const char *at, size_t length){
  RequestContext* reqCon = (RequestContext*)p->data;
  reqCon->req->url.assign(at, length);
  return 0;
}

int on_headers_complete_func(http_parser * p) {
  
  return 0;
}

int on_body_func(http_parser * p, const char *at, size_t length){

  return 0;
}

void uv_write_file_cb_func(uv_write_t* req, int status){
  if(sbuf.base) {
    delete [] sbuf.base;
    memset(&sbuf, 0, sizeof(sbuf));
    sbuf.base = nullptr;
  }
  RequestContext* reqCon = (RequestContext*)req->data;
  send_document_in_chunked("http/404.html", reqCon, uv_write_cb_func);
}

void send_document_in_chunked(std::string path, RequestContext* reqCon, uv_write_cb cb) {
  static std::ifstream fsrm(path);
  std::string str;
  if(!fsrm.is_open()) {
    fsrm.open(path);
  }
  std::getline(fsrm, str);
  if(str.size()) {
    char len[10] = "";
    sprintf(len,"%x", (unsigned int)str.size());
    std::string res(len);
    res += "\r\n" + str + "\r\n";
    if(sbuf.base) {
      delete [] sbuf.base;
    }
    sbuf.base = new char[res.size()];
    memcpy(sbuf.base, res.c_str(), res.size());
    sbuf.len = res.size();
    memset(reqCon->write_req, 0, sizeof(sizeof(uv_write_t)));  
    reqCon->write_req->data =  reqCon; 
    uv_write(reqCon->write_req, (uv_stream_t *)reqCon->tcp, &sbuf, 1, uv_write_file_cb_func);
  } 
  else {
    fsrm.close();
    if(sbuf.base) {
      delete [] sbuf.base;
    }
    sbuf.base = new char[5]();
    memcpy(sbuf.base, "0\r\n\r\n", 5);
    sbuf.len = strlen(sbuf.base);
    memset(reqCon->write_req, 0, sizeof(sizeof(uv_write_t)));
    reqCon->write_req->data =  reqCon;   
    uv_write(reqCon->write_req, (uv_stream_t *)reqCon->tcp, &sbuf, 1, cb);
  }
}

int on_message_complete_func(http_parser * p){
  RequestContext* reqCon = (RequestContext*)p->data;
  HttpRequest* req = reqCon->req;
  Net* net = req->net;
  radix_tree* tree = net->route_tree;
  routeFunc reqHand = (routeFunc)route(tree, req->url.c_str());
  if(!reqHand) {
    sbuf.len = strlen(CHUNKED_HEAD);
    sbuf.base = new char[sbuf.len];
    memcpy(sbuf.base, CHUNKED_HEAD, sbuf.len); 
    memset(reqCon->write_req, 0, sizeof(sizeof(uv_write_t)));  
    reqCon->write_req->data = reqCon;
    uv_write(reqCon->write_req,(uv_stream_t *)reqCon->tcp, &sbuf, 1, uv_write_file_cb_func);
    // send_document_in_chunked("http/404.html", reqCon, uv_write_cb_func);
  } else {
    std::string response = reqHand(reqCon);
    sbuf.len = response.size();
    sbuf.base = new char[sbuf.len];
    memcpy(sbuf.base, response.c_str(), sbuf.len);
    reqCon->end_signal = true;
    // int siz = response.size();
    // sbuf.base = (char *)malloc(siz);
    // sbuf.len = siz;
    // memcpy(sbuf.base, response.c_str(), siz);
    memset(reqCon->write_req, 0, sizeof(uv_write_t));  
    reqCon->write_req->data =  reqCon; 
    uv_write(reqCon->write_req,(uv_stream_t *)reqCon->tcp, &sbuf, 1, uv_write_cb_func);
  }
  return 0;
}

void init_http_settings(http_parser_settings* settings) {
  settings->on_body = on_body_func;
  settings->on_header_field = on_header_field_func;
  settings->on_header_value = on_header_value_func;
  settings->on_headers_complete = on_headers_complete_func;
  settings->on_message_begin = on_message_begin_func;
  settings->on_message_complete = on_message_complete_func;
  settings->on_status = on_status_func;
  settings->on_url = on_url_func;
}