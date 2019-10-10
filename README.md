🥜🥐🥖🥞🥓🥙🥚🥘🥗🥫🥣🥪🥩🥨🥦🥥🦗🥛🥟🥠🥡🥤🥢🥧 🥦🥩🥐🍕🎯🥫🥧

&emsp;基于libuv的一个简单C++ http服务器 \
&emsp;
不断完善中。

### 🍕 finished：
+ 静态路由
+ 多种文件格式mime传输
+ 连接mysql

###  🎯 to do:
+ 时间轮(tcp::collectionList)关闭长久不用的连接
+ 路由匹配$等变量值(/url/$abc/col?a=10)
+ 复杂静态路径的处理(/root/file/*.jpg或者文件不存在)
+ keep-alive
+ Accept-Ranges: bytes
&ensp;

### 🗼 第三方依赖
+ [libuv](https://github.com/libuv/libuv)
+ [json](https://github.com/nlohmann/json)
+ [mysql-connector-cpp](https://github.com/mysql/mysql-connector-cpp)

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

