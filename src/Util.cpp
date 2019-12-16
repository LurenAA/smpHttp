#include "Util.hpp"
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <codecvt>
#include <locale>
#include "uv.h"

using namespace std;
using namespace xx;

#define CASE(x,y, t) \
  t->insert({#x, y}); \

shared_ptr<map<string, string>> initialUtilMimes() {
  shared_ptr<map<string, string>>  res = make_shared<map<string, string>>();
  MIME_INIT(CASE, res)
  return res;
}
shared_ptr<std::map<std::string, std::string>> Util::mimes = initialUtilMimes();

void Util::trim(std::string& s) {
  if( !s.empty() )
  {
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1, s.size());
  }
}

std::string Util::getRootPath() {
  string path(getcwd(nullptr, 0));
  return path;
}

string Util::getMime(const std::string& s){
  try{
    auto res = Util::mimes->at(s);
    return res;
  } catch(...){
    return "";
  }
}

std::string Util::getExt(const std::string &s) {
  auto pos = s.find_last_of('.');
  if(pos == string::npos) {
    return "";
  }
  return s.substr(pos + 1);
}

bool Util::starts_with(const std::string& sou, const std::string& key){
  if(sou.size() < key.size())
    return false;
  auto p = mismatch(sou.cbegin(), sou.cend(), key.cbegin());
  if(p.second == key.cend()) {
    return true;
  } 
  return false;
}

std::string Util::join(const std::vector<std::string>& vec, char j){
  string rs;
  for(auto x: vec) {
    rs += x + j;
  }
  rs.erase(rs.end() - 1);
  return rs;
}

std::vector<std::string> Util::split(const std::string& str, char j) {
  vector<string> str_vec;
  string::size_type pos = 0,
    pre_pos = 0;
  while(1) {
    pos = str.find(j, pre_pos);
    string s(str.substr(pre_pos, pos - pre_pos));
    str_vec.push_back(s);
    if(pos == string::npos) {
      break;
    }
    pre_pos = pos + 1;
  }
  return str_vec;
}

std::string Util::utf16Toutf8(const mysqlx::Value& v) {
  if(v.isNull()) 
    return "";
  mysqlx::string s = v;
  return std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t >{}.to_bytes(s.c_str());
}

bool Util::isBase64(const std::string& s) {
  static const std::string beg_str("data:");
  for(int i = 0; i < 5; ++i) {
    if(beg_str[i] != beg_str[i]) {
      return false;
    }
  }
  auto x = s.find("base64");
  if(x == string::npos) {
    return false;
  }
  return true;
}

std::string Util::getStaticDirnameUrl(const std::string& host, int port) {
  std::string res = "";
  auto hasPre = host.find("http://", 0, sizeof("http://"));
  if(hasPre == string::npos) {
    res = "http://";
  }
  return res += host + ":" + to_string(port) + '/';
}

std::string Util::get_strerror_r(int errno_r) {
  auto& tl = Tools::getInstance();
  return tl.get_strerror_r(errno_r);
}

std::string Util::get_uv_strerror_t(int errno_r) {
  auto& tl = Tools::getInstance();
  return tl.get_uv_strerror_t(errno_r);
}

/**
 * 用于路由中将请求的前缀和路径结合到一起，
 * 在in_read_second中有用到
 **/ 
std::string Util::conbine_prefix_and_path(std::string prefix , std::string path) 
{
  if(!prefix.size())
    return path;
  if(prefix.back() != '/' && path.front() != '/') {
    prefix += '/';
  }
  string res = prefix + path;
  auto itr = res.find("//");
  if(itr != string::npos) {
    res.erase(itr, itr + 2);
  }
  return res;
}
#undef CASE