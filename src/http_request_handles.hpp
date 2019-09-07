#ifndef __asdasd_c
#define __asdasd_c
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "http_parser.h"
#include "uv.h"
struct RequestContext;

void init_http_settings(http_parser_settings* settings);
void send_document_in_chunked(std::string, RequestContext*, uv_write_cb );

int on_message_begin_func(http_parser *);
int on_header_value_func(http_parser *, const char *at, size_t length);
int on_header_field_func(http_parser *, const char *at, size_t length);
int on_status_func(http_parser *, const char *at, size_t length);
int on_url_func(http_parser *, const char *at, size_t length);
int on_headers_complete_func(http_parser *);
int on_body_func(http_parser *, const char *at, size_t length);
int on_message_complete_func(http_parser *);
void uv_write_file_cb_func(uv_write_t* req, int status);
#endif