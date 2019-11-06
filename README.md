🥜🥐🥖🥞🥓🥙🥚🥘🥗🥫🥣🥪🥩🥨🥦🥥🦗🥛🥟🥠🥡🥤🥢🥧 🥦🥩🥐🍕🎯🥫🥧

&emsp;基于libuv的一个简单C++ http服务器 \
&emsp;

### 🍕 finished：
+ 静态路由
+ 多种文件格式mime传输
+ 连接mysql
+ 返回json请求
+ jwt
+ Cors
+ 仿照[webcc](https://github.com/sprinfall/webcc/blob/master/webcc/router.h) 的方式做路由，代替radix_tree

###  🎯 to do:
+ 路由匹配$等变量值(/url/$abc/col?a=10)
+ 复杂静态路径的处理(/root/file/*.jpg或者文件不存在)
+ keep-alive
+ Accept-Ranges: bytes
+ 检查数据报格式是否正确
+ 使用base64来传输图片文件
+ 定时处理没响应的连接
+ 完全使用shared_ptr来管理连接
&ensp;

### 🗼 第三方依赖
+ [libuv](https://github.com/libuv/libuv)
+ [json](https://github.com/nlohmann/json)
+ [mysql-connector-cpp](https://github.com/mysql/mysql-connector-cpp)
+ [jwt-cpp](https://github.com/pokowaka/jwt-cpp)

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

