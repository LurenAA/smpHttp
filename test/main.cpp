#include "route.hpp"

extern 
mysqlx::Client cli;
extern 
std::string host;
extern 
int port;


//帮助信息
#define HELP_INFO \
"Usage: smpHttp [OPTION...]\n" \
"-p, --port=PORT_NUMBER\t\t指明端口号,必须和前端保持一致,默认值为8080\n" \
"-H, --host=HOST_NAME  \t\t用于外部访问的域名或者IP,否则存在数据库中存放的静态资源路径为相对路径，\n"\
"                      \t\t只有挂载在本地的前台可以访问图片文字资源\n" \
"-h, --help            \t\t显示帮助" 
int _help = 0; //记录命令行参数状态
static 
const struct option long_option[]={
   {"port",optional_argument,NULL,'p'},
   {"host",optional_argument,NULL,'H'},
   {"help",no_argument, &_help, 1},
   {NULL,0,NULL,0}
};

int main(int argc, char * argv[]) {
  int opt=0;
  while((opt=getopt_long(argc,argv,"p:H:h",long_option,NULL))!=-1)
  {
    switch(opt)
    {
      case 0: break;
      case 'H': 
        host = optarg;
        break;
      case 'p':
        port = atoi(optarg);
        if(port == 0) 
          _help = 1; 
        break;                       
      case 'h': 
        _help = 1;
        break;
      case '?':
        std::cout << "错误的参数" << std::endl;
        _help = 1;
        break;
    }
    if(_help == 1) 
      break;
  }
  if(_help == 1) {
    std::cout << HELP_INFO << std::endl;
    return 1;
  }
  smpHttp::HttpServer server;
  server.setPort(port);
  server.add_route("^/json_lab$", handle_json_lab);
  server.add_route(R"(^/json_news(\?([^/\?=&]+=[^/\?=&]+&)*[^/\?=&]+=[^/\?=&]+)?)", handle_json_news);
  server.add_route("^/login$", handle_login);
  server.add_route("^/.*", handle_base); //need to change
  server.add_route("^/get_members", get_members); 
  server.add_static_path(R"(^/resources.*)"); //add static route
  server.add_route("^/member_change$", handle_member_change);
  server.add_route("^/assets_change$", change_assets);
  server.add_route("^/get_assets",get_assets);
  server.add_route("^/get_articles",get_articles);
  server.add_route("^/change_articles", change_articles);
  server.add_route("^/get_assets_inf",get_assets_inf);
  server.run();
  cli.close();
  return 0;
}