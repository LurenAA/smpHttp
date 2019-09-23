#include "HttpParser.hpp"
#include <string>
#include <algorithm>

using namespace std;
using namespace hpr;

string trimString(string::const_iterator,string::const_iterator);

HttpResult* HttpParser::handleDatagram(const std::string& datagram) {
  auto iter = datagram.cbegin(), 
    iend = datagram.cend();
  HttpResult* res = new HttpResult();
  parseMethod(iter, iend, res);
  parseRequestTarget(iter, iend, res);
  parseHttpVersion(iter, iend, res);
  parseHeaders(iter, iend, res);

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
  while (*iter != '\r')
  {
    auto im = find(iter, iend, ':');
    auto ie = find(iter, iend, '\r');
    string name = trimString(iter, im),
      value = trimString(im + 1, ie);
    res->headers.insert({name, value});
    iter = ie + 2;
  }  
}

string trimString(string::const_iterator a,string::const_iterator b) {
  auto iter = *a == ' ' ? a + 1: a;
  auto iend = *(b - 1) == ' ' ? b - 1 : b;
  return string(iter, iend); 
}