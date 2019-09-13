#include "HttpParser.hpp"
#include <string>
#include <algorithm>

using namespace std;
using namespace hpr;

shared_ptr<HttpResult> HttpParser::handleDatagram(const std::string& datagram) {
  auto iter = datagram.cbegin();
  shared_ptr<HttpResult> res;

  return res;
}

shared_ptr<HttpResult> HttpParser::handleDatagram(const char* str, int len) {
  string datagram(str, len);
  return handleDatagram(datagram);
}

void HttpParser::parseMethod(std::string::const_iterator& iter,
  const std::string::const_iterator& iend, 
  shared_ptr<HttpResult>& res) 
{
  auto firstSpace = find(iter, iend, ' ');
  
}