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
+ 完全使用shared_ptr来管理连接
+ 使用base64来传输图片文件

###  🎯 to do:
+ 路由匹配$等变量值(/url/$abc/col?a=10)
+ 复杂静态路径的处理(/root/file/*.jpg或者文件不存在)
+ keep-alive
+ Accept-Ranges: bytes
+ 检查数据报格式是否正确
+ 定时处理没响应的连接
+ 对于DELETE、PUT等等方法的处理
&ensp;

### 🥘 配置
```
###📣 代码相关的库可以在本仓库wiki中的百度云盘链接下载
cmake . -DNLO_PATH=src/nlohmann_json -DMYSQL_PATH=src/modules/mysql-connector-cpp/libmysqlcppconn8-static.a
make
```
-DNLO_PATH=...指定[json](https://github.com/nlohmann/json)库的位置，默认是src/nlohmann_json <br>
-DMYSQL_PATH=...指定[mysql-connector-cpp](https://github.com/mysql/mysql-connector-cpp)链接库的位置
### 🗼 第三方依赖
+ [libuv](https://github.com/libuv/libuv)
+ [json](https://github.com/nlohmann/json)
+ [mysql-connector-cpp](https://github.com/mysql/mysql-connector-cpp)
+ [jwt-cpp](https://github.com/pokowaka/jwt-cpp)

### 🚇 simple example

```
void get_collection(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, RouteWq &wq)
{
}

int main(int argc, char *argv[])
{
  int port = 8080;
  HttpServer server(port);
  server.add_route("/collection", get_collection, xx::GET);
  server.add_static_route("/resources.*");
  server.run();
  return 0;
}
```

