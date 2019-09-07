#ifndef __HTTP_REQUEST_HPP_
#define __HTTP_REQUEST_HPP_
#include <string>
class Net;

struct HttpRequest {
  HttpRequest(Net* aNet = nullptr): net(aNet) {}
  void setNet(Net* net);
  std::string url;
  Net* net;
};

#endif