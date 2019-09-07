#ifndef __UV_HANDLES_H_
#define __UV_HANDLES_H_
#include "uv.h"
#include <iostream>
struct RequestContext;

void uv_connection_cb_func(uv_stream_t* server1, int status);
void uv_alloc_cb_func(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void uv_read_cb_func(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void uv_close_cb_func(uv_handle_t* handle);
void uv_write_cb_func(uv_write_t* req, int status);
void release(RequestContext* reqCon);
#endif