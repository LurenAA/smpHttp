#include "HttpServer.hpp"
#include <cstring>
#include <iostream>
#include <iterator>
#include <functional>
#include <unistd.h>
#include <algorithm>
#include <string>
#include "HttpConnection.hpp"

using namespace std;
using namespace xx;
using namespace std::placeholders;

// void HttpServer::afterRead(std::shared_ptr<uvx::Connection> cl){
//   HttpConnection* hcl = dynamic_cast<HttpConnection*>(cl.get());
//   #ifdef DEBUG_FLAG
//     cout << "datagram: " << "\r\n" 
//     << hcl->getDatagram() << endl;
//   #endif

//   try {
//     HttpResult* r = parser.handleDatagram(hcl->getDatagram());
//     if(!r) {
//       cout << "error : " << __func__ << 
//       " r is empty " << endl;
//       cl->close();  
//     }
//     shared_ptr<HttpRequest> parseReq(new HttpRequest(*r ,cl, cl->_loop()));
//     //to complete
//     if(parseReq->getMethod() == hpr::OPTIONS) {
//       shared_ptr<HttpResponse> res = newHttpResponse(cl);
//       res->setAfterWrite(bind([](shared_ptr<Connection> cl){
//         cl->close();
//       }, cl));
//       res->addHeader("Access-Control-Allow-Origin", parseReq->getHeader("Origin"));
//       res->addHeader("Access-Control-Allow-Methods","POST, GET, OPTIONS");
//       res->addHeader("Access-Control-Max-Age","86400");
//       res->addHeader("Connection","keep-alive");
//       res->addHeader("Access-Control-Expose-Headers", "token");
//       res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept, token");
//       return ;
//     }
//     return handleRoute(parseReq, cl);
//   } catch (std::exception& e){
//     cl->close();
//     cout<< "warn:" << __FILE__ << ": " 
//       << __LINE__ <<" ：" <<  e.what() << endl;
//   } 
// };

// void HttpServer::handleRoute(shared_ptr<HttpRequest> parseReq, shared_ptr<Connection> cl) {
//   bool if_static_path = route.get_static_route(parseReq->getRequestPath());
//   shared_ptr<HttpResponse> parseRes = newHttpResponse(cl);

//   if(if_static_path) {
//     parseRes->addHeader("Access-Control-Allow-Origin", parseReq->getHeader("Origin"));
//     parseRes->addHeader("Access-Control-Allow-Methods","POST, GET, OPTIONS");
//     parseRes->addHeader("Access-Control-Max-Age","86400");
//     parseRes->addHeader("Connection","keep-alive");
//     parseRes->addHeader("Access-Control-Expose-Headers", "token");
//     parseRes->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept, token");
//     deal_with_static(parseReq,parseRes);
//   } else {
//     vector<routeHandleFunc> funcs = route.get_route(parseReq->getRequestPath());
//     parseRes->setAfterWrite(bind([](shared_ptr<Connection> cl){
//       cl->close();
//     }, cl));
//     for(auto x : funcs) {
//       x(parseReq,parseRes); 
//       if(parseRes->isEnd())
//         break;
//     }
//     if(!funcs.size()) {
//       parseRes->addMessage("no such route");
//     }
//   }
// }

// void HttpServer::add_route(std::string s, routeHandleFunc func){
//   route.add_route(s, func);
// }

// void HttpServer::add_static_path(std::string s) {
//   route.add_static_route(s);
// }

// void HttpServer::deal_with_static(std::shared_ptr<HttpRequest> req
//     , std::shared_ptr<HttpResponse>& res)
// {
// #define CHUNK_SIZE 50000
//   res->setAfterWrite(nullptr);
//   if(!req->fstream){
//     //never use it before
//     shared_ptr<IfstreamCon> newF(make_shared<IfstreamCon>());
//     newF->open(Util::getRootPath() + req->getRequestPath());

//     if(!newF->is_open()) {
//       //to do with error
//       std::string s = "errno:" +  req->getRequestPath() 
//       + " does not exist " +  __FILE__ + " " + to_string(__LINE__);
//       cout << s << endl;
//       res->addMessage(s);
//       res->setAfterWrite(nullptr);
//       res->close();
//       return ;
//     }
//     req->fstream = newF;
//     //check the type
//     res->setContentType(req->getRequestPath());
//   }


//   if(req->fstream) {
//     shared_ptr<IfstreamCon> fstrm = req->fstream;
//     if(!fstrm->is_open()) {
//       res->setAfterWrite(nullptr);
//       cout << "log: close " << req->getRequestPath()  << endl ;
//       //to do time wheel
//       res->close();
//       return ;
//     }
//     string s;
//     fstrm->read(s, CHUNK_SIZE); //read from the fstream
//     res->addMessage(s); //add message body
//     bool if_set_next_func = false; //if need a next deal_with_static call
//     IfstreamCon::size_type remain = fstrm->remainNum();
//     if(remain == 0) {
//       /**
//        * for normal datagram
//        **/ 
//       if(res->getMode() == NORMAL && res->isFirst()) { 
//         res->addHeader("Content-Length", to_string(s.size()));
//       } 
//       /**
//        * for the last chunk 
//        **/ 
//       else if(res->getMode() == CHUNKED && !res->isFirst()){
//         res->setLastChunked();
//       } else {
//         cout << "error:" << to_string(__LINE__) <<
//         res->getMode() << " " << res->isFirst() << endl;
//         return ;
//       }
//     } else {
//       /**
//        * for the first chunked one
//        **/ 
//       if(res->isFirst()) {
//         res->setNotFirst();
//         res->setMode(CHUNKED_FIRST);
//       }
//       /**
//        * for a normal chunked one
//        **/  
//       else {
//         res->setMode(CHUNKED);
//       }
//       if_set_next_func = true;
//     }
    
//     shared_ptr<HttpResponse> rres = newHttpResponse(*res);
//     if(if_set_next_func) {
//       if(res->getMode() == CHUNKED || res->getMode() == CHUNKED_FIRST) 
//         rres->setMode(CHUNKED);
//       res->setAfterWrite(bind(&HttpServer::deal_with_static, this, req, rres));    
//     } else{
//       /**
//        * close the connection next loop
//        **/ 
//       fstrm->close();
//       auto wfunc = bind(&HttpServer::deal_with_static, this, req, rres);
//       res->setAfterWrite(wfunc);  
//     }
//   } 
// #undef CHUNK_SIZE
// }

// void HttpServer::deal_with_error(std::shared_ptr<HttpRequest> req,
//   std::shared_ptr<HttpResponse> res,const std::string& s)
// {
//   res->addMessage(s);
// }

HttpServer::HttpServer(AddressInfo a, EventLoop* lp) :
  _lp(lp), _tcpServer(*_lp, a), _route()
{
  auto af_con = bind(&HttpServer::after_connection, this, _1);
  _tcpServer.setAfterConnectionCb(af_con);
}

HttpServer::~HttpServer(){
  if(!_lp->is_active())
    _lp->close();
  delete _lp;
}

void HttpServer::run() {
  _tcpServer.listen();
  _lp->run();
}

void HttpServer::after_connection(std::shared_ptr<TcpConnection> tc) {
  tc->read();
  auto in_rd = bind(&HttpServer::in_read, this, _1, _2, _3, _4);
  tc->setInReadCb(in_rd);
}

void HttpServer::in_read_second(std::shared_ptr<TcpConnection> tc) {

}

void HttpServer::in_read(std::shared_ptr<TcpConnection> tc, ssize_t nread, const uv_buf_t *buf, bool isEof) {
  HttpConnection* cn = dynamic_cast<HttpConnection*>(tc.get());
  ssize_t grm = cn->getRemain();
  Method mtd = cn->getMethod();
  string& rfd = cn->getReserveForRead();
  auto& fl = FileLog::getInstance();
  auto& tl = Tools::getInstance();
  try {
    if(isEof) {
      if(grm > 0) {
        fl.error("remain is " + grm, __func__, __FILE__, __LINE__);
      }
      cn->close();
      return;
    }
    if(mtd == Method::NOTSET && grm == INT_MIN) { 
      /**
       * 第一次开始接受数据报
       **/ 
      char* method_end = strstr(buf->base, " ");
      if(!method_end) {
        fl.error("onStartRead -> method_end == null", __func__, __FILE__, __LINE__);
        cn->close();
        return ;
      }
      string method = string(buf->base, method_end);
      /**
       * 判断方法
       **/ 
      Method mcp;
      if(method == "POST") {
          mcp = Method::POST;
      } else if (method == "GET") {
          mcp = Method::GET;
      } else if(method == "CONNECT") {
        mcp = Method::CONNECT;
      } else if (method == "DELETE") {
        mcp = Method::DELETE;
      } else if (method == "HEAD") {
        mcp = Method::HEAD;
      } else if (method == "OPTIONS") {
        mcp = Method::OPTIONS;
      } else if(method == "PUT") {
        mcp = Method::PUT;
      } else if (method == "TRACE"){
        mcp = Method::TRACE;
      } else {
        mcp = Method::NOTSET;
      }
      cn->setMethod(mcp);
    }
    
    rfd += std::string(buf->base, nread);
    /**
     * post
     **/ 
    if(mtd == Method::POST) {
      if(grm == INT_MIN) {
        char* l = strstr(buf->base, "Content-Length:");
        if(!l) {
          fl.error("strstr: " + tl.get_strerror_r(errno), __func__, __FILE__,__LINE__);
          cn->close();
          return ;
        }
        char* r = strchr(l, '\r');
        if(!r){
          fl.error("strchr: " + tl.get_strerror_r(errno), __func__, __FILE__,__LINE__);
          cn->close();
          return ;
        }
        string len_s(l + strlen("Content-Length:"), r);
        cn->setRemain(atoi(len_s.c_str()));
        char *d = strstr(buf->base, "\r\n\r\n");
        if(d)
          cn->setRemain(cn->getRemain() + d - buf->base + 4);
      }
      cn->setRemain(cn->getRemain() - nread);
      if(cn->getRemain() <= 0) {
        this->in_read_second(tc);
        return ;
      } 
    }
    /**
     * get / options
     **/  
    else if (cn->getMethod() == Method::GET || cn->getMethod() == Method::OPTIONS) {
      cn->setRemain(0);
      if(rfd.back() == '\n' && *(rfd.end() - 2) == '\r'
      && *(rfd.end() - 3) == '\n' && *(rfd.end() - 4) == '\r'
      )
      {
        this->in_read_second(tc);
      } else {
        fl.error(R"(error in "get" or "options")", __func__, __FILE__,__LINE__);
        return ;
      }
    }
    /**
     * other methods
     **/ 
    else {

    }
  } catch(exception& e) {
    fl.error(string("HttpServer::in_read : ") + e.what(), __func__, __FILE__, __LINE__);
    cn->close();
  }
}

TcpConnection* HttpAccepter::tcp_connection_object() {
  return new HttpConnection(*this, _lp);
}