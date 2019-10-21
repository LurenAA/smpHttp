#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_
#include "uvx.hpp"
#include "RadixTree.hpp"
#include "HttpParser.hpp"
#include <fstream>
#include <map>
#include <functional>
#include "IfstreamCon.hpp"
#include <set>
#include "Route.hpp"


namespace smpHttp {
  class HttpResponse;
  class HttpRequest;
  class HttpServer : public uvx::Tcp {
    public:
      using uvx::Tcp::Tcp;
      void add_route(std::string s, routeHandleFunc);
      void add_static_path(std::string s);

    private:
      hpr::HttpParser parser;
      Route route;

      void deal_with_static(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>);
      void deal_with_error(std::shared_ptr<HttpRequest>,std::shared_ptr<HttpResponse>,const std::string&);

      void handleRoute(std::shared_ptr<HttpRequest> parseRes, uvx::Connection* cl);
      void afterRead(uvx::Connection* cl);

      void theConnectionCallback(uvx::Connection*) override;
      void theListenCallback() override;
  };
}
#endif //__HTTPSERVER_H_ 