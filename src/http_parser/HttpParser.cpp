#include "HttpParser.hpp"
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace hpr;

string trimString(string::const_iterator,string::const_iterator);

HttpResult* HttpParser::handleDatagram(const std::string& datagram) {
  auto iter = datagram.cbegin(), 
    iend = datagram.cend();
  HttpResult* res = new HttpResult();
  try {
    parseMethod(iter, iend, res);
    parseRequestTarget(iter, iend, res);
    parseHttpVersion(iter, iend, res);
    parseHeaders(iter, iend, res);

    parseQueries(res);
    parseRequestPath(res);
  } catch(exception& e){
    throw HttpParserError("error datagram format");
  } 
  
  return res;
}

HttpResult* HttpParser::handleDatagram(const char* str, int len) {
  string datagram(str, len);
  return handleDatagram(datagram);
}

void HttpParser::parseMethod(string::const_iterator& iter,
  string::const_iterator& iend, 
  HttpResult* res) 
{
  auto firstSpace = find(iter, iend, ' ');
  res->setMethod(iter, firstSpace);
  iter = firstSpace + 1;
}

void HttpParser::parseRequestTarget(string::const_iterator& iter,
  string::const_iterator& iend, 
  HttpResult* res)
{
  auto firstSpace = find(iter, iend, ' ');
  res->setRequestTarget(iter, firstSpace);
  iter = firstSpace + 1;
}

void HttpParser::parseHttpVersion(string::const_iterator& iter,
  string::const_iterator& iend, 
  HttpResult* res)
{
  auto firstSpace = find(iter, iend, '\r');
  res->setHttpVersion(iter, firstSpace);
  iter = firstSpace + 2;
}

void HttpParser::parseHeaders(std::string::const_iterator& iter,
  std::string::const_iterator& iend, 
  HttpResult* res)
{
  while (iter != iend && *iter != '\r')
  {
    auto im = find(iter, iend, ':');
    auto ie = find(iter, iend, '\r');
    string name = trimString(iter, im),
      value = trimString(im + 1, ie);
    res->headers.insert({name, value});
    iter = ie + 2;
  }   
}

void HttpParser::parseQueries(HttpResult* res)
{
  string tar = res->getRequestTarget();
  auto beg = tar.find_first_of('?');
  if(beg == string::npos) {
    return ;
  }
  tar = tar.substr(beg + 1);
  string key, value;
  auto max_size = tar.size();
  decltype(max_size) cur = 0;
  while(cur < max_size) {
    auto k = tar.find_first_of('=', cur);
    auto v = tar.find_first_of('&', cur);
    if(k == string::npos) {
      cout << "log:" << __FILE__ << " : " << 
        __LINE__ << "query reach end" << endl;
      return ;
    }
    res->queries.insert({tar.substr(cur, k - cur), tar.substr(k + 1, v - k -1)});
    if(v == string::npos)
      break;
    cur = v >= max_size ? max_size : v + 1;
  }  
}

string trimString(string::const_iterator a,string::const_iterator b) {
  auto iter = *a == ' ' ? a + 1: a;
  auto iend = *(b - 1) == ' ' ? b - 1 : b;
  return string(iter, iend); 
}

void HttpParser::parseRequestPath(HttpResult* res){
  string tar = res->getRequestTarget();
  auto ie = tar.find_first_of('?');
  res->setRequestPath(tar.substr(0, ie - 0));
}