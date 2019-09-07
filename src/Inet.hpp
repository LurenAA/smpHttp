#ifndef __INET_HPP_
#define __INET_HPP_
#include <string>
struct RequestContext;
typedef std::string (*routeFunc)(RequestContext*);
/**
 * Inet接口，明确Net实现的功能
 **/ 
class Inet {
public: 
  virtual bool add_route(const std::string& path, routeFunc func) = 0;
  virtual void change_ip(std::string ) = 0;
  virtual void change_port(int) = 0;
  virtual void changeBackLog(int) = 0; 
  virtual void start_server() = 0;
  virtual ~Inet() {}
};

#endif