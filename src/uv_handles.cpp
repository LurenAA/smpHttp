#include "uv_handles.hpp"
#include "RequestContext.hpp"
#include "Net.hpp"
#include "HttpRequest.hpp"


http_parser_settings settings;

/**
 * listen后的回调函数
 */
void uv_connection_cb_func(uv_stream_t* server1, int status){
  if(status < 0) {
    fprintf(stderr, "New connection error %s\n", uv_strerror(status));
    return;
  }
  RequestContext* reqCon = new RequestContext();
  Net* net = (Net*)server1->data;
  reqCon->req->setNet(net);
  if(!reqCon) 
    return;
  if(!uv_accept((uv_stream_t *)server1, (uv_stream_t *)reqCon->tcp)){
    uv_read_start((uv_stream_t *)reqCon->tcp,uv_alloc_cb_func, uv_read_cb_func);
  } else {
    uv_close((uv_handle_t *)reqCon->tcp, uv_close_cb_func);
  }
}

/**
 * read_start后内存的申请回调函数
 */
void uv_alloc_cb_func(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf){
  *buf = uv_buf_init((char*)malloc(sizeof(char)* suggested_size), suggested_size);
}

/**
 * read_start后开始读取流的回调函数
 */
void uv_read_cb_func(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf){
  bool close_flag = false;
  if(nread < 0) {
    if(nread != UV_EOF) {
      fprintf(stderr ,"read error in uv_read_cb_func: %s:%s\n"
        ,uv_err_name(nread), uv_strerror(nread));
    }
    close_flag = true;
  }
  else {
    RequestContext* reqCon = (RequestContext*)stream->data;
    int nparsed = http_parser_execute(&reqCon->parser, &settings, buf->base, nread);
    if(nparsed < nread) {
      std::cout << "error: http_parser_execute error" << std::endl;
      close_flag = true;
    }
  }
  if(close_flag)
    uv_close((uv_handle_t *)stream, uv_close_cb_func);

  if(buf)
    free(buf->base);
}

/**
 * accept后释放连接资源回调函数
 */
void uv_close_cb_func(uv_handle_t* handle){
  RequestContext* reqCon = (RequestContext*)handle->data;
  release(reqCon);
}

/**
 * 报文发送结束后
 */
void uv_write_cb_func(uv_write_t* req, int status){
  RequestContext* reqCon = (RequestContext*)req->data;
  if(status < 0) {
    fprintf(stderr, "%s:%s\n", uv_err_name(status), uv_strerror(status));
    return ; 
  }
  if(reqCon->end_signal && !uv_is_closing((uv_handle_t *)reqCon->tcp)) {
    uv_close((uv_handle_t *)reqCon->tcp, uv_close_cb_func);
  }
}

/**
 * 释放一个request_context对象的资源，
 * 这个函数在uv_close了reqCon->tcp后
 * 使用
 */
void release(RequestContext* reqCon){
  if(!reqCon)
    return;
  delete reqCon;
}