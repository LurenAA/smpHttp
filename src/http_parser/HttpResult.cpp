#include "HttpResult.hpp"
#include <algorithm>
#include <vector>

using namespace hpr;
using namespace std;

HttpResult::HttpResult(HttpResult* s) :
  method(s->method), requestTarget(s->requestTarget)
  , httpVersion(s->httpVersion), headers(s->headers)
{
}

void HttpResult::setMethod(const string& str) {
  MethodValue(CASE, method, str);
}

void HttpResult::setMethod(string::const_iterator& iter
  , string::const_iterator& iend)
{
  setMethod(string(iter, iend));
}

void HttpResult::setRequestTarget(string::const_iterator& iter, 
  string::const_iterator& iend)
{
  requestTarget.assign(iter, iend);
}

void HttpResult::setHttpVersion(string::const_iterator& iter, 
  string::const_iterator& iend)
{
  static vector<string> versions{"1.1", "1.0", "0.9"};
  string::const_iterator fd;
  for(auto i : versions) {
    fd = search(iter, iend, i.cbegin(), i.cend());
    if(fd != iend) {
      httpVersion = atof(i.c_str());
      return ;
    }
  }
  httpVersion = 1.1;  
}