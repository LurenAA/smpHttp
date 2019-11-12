#ifndef _ROUTE_HPP__
#define _ROUTE_HPP__
#include <iostream>
#include <string>
#include "smpHttp.hpp"
#include <nlohmann/json.hpp>
#include <codecvt>
#include <locale>
#include "jwt/jwt_all.h"
#include "stdlib.h"
#include <execinfo.h>
#include <exception>
#include <ctime>
#include <unistd.h>
#include <algorithm>
#include "Base64Decoder.hpp"

typedef struct {
  uv_buf_t* buf;
  std::string* str;
  int fd;
} buf_and_data;

extern 
mysqlx::Client cli;

using Route_Type = void (std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res);

Route_Type handle_json_lab;
Route_Type handle_json_news;
Route_Type handle_base;
Route_Type handle_member_change;
Route_Type handle_login;
Route_Type get_members;
Route_Type change_assets;
Route_Type get_assets;
Route_Type change_articles;
Route_Type get_articles;

#endif