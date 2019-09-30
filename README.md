🥜🥐🥖🥞🥓🥙🥚🥘🥗🥫🥣🥪🥩🥨🥦🥥🦗🥛🥟🥠🥡🥤🥢🥧 🥦🥩🥐🍕🎯🥫🥧

&emsp;基于libuv的一个简单C++ http服务器 \
&emsp;
不断完善中。

### 🍕 finished：
+ 静态路由
+ 多种文件格式传输

###  🎯 to do:
+ 时间轮(tcp::collectionList, HttpServer::fstreamMap)
+ 路由匹配$等变量值
+ 复杂静态路径的处理
+ keep-alive
+ Accept-Ranges: bytes
&ensp;

### 🗼 第三方依赖
+ [libuv](https://github.com/libuv/libuv)
+ [json](https://github.com/nlohmann/json)
+ [mysql-connector-cpp](https://github.com/mysql/mysql-connector-cpp)
+ [boost](https://www.boost.org/)


### 🚇 simple example

```
#include <iostream>
#include "smpHttp.hpp"
using namespace std;

void test1(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  res->addMessage("hello world");
}

int main() {
  smpHttp::HttpServer server;
  server.add_static_path("/http"); //add static route
  server.add_route("/hello", test1);
  server.run();
  
  return 0;
}
```