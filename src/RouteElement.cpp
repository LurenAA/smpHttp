#include "RouteElement.hpp"
#include <typeinfo>
#include "RouteWq.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpServer.hpp"
#include "uv.h"
#include "FsWork.hpp"
#include <functional>

using namespace std;
using namespace xx;
void static_file_handle_open(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, RouteWq &wq, shared_ptr<FsWork> wk);

void static_file_handle_read(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, RouteWq &wq, shared_ptr<FsWork> wk)
{
  /**
  * 小于默认限制大小
  **/
  if (static_cast<int64_t>(wk->getStatBuf().st_size) < wk->getMaxSizeDirectRead())
  {
    wk->getRes()->setMode(TransMode::NORMAL);
  }
  /**
   * 超过大小
  **/
  else
  {
    /**
     * 设置传输模式
     **/ 
    if(!wk->getRead()) {
      wk->getRes()->setMode(TransMode::CHUNKED_START);
    } else 
      wk->getRes()->setMode(TransMode::CHUNKED);
  }
  wk->enable_buf(DEFAULT_MAX_SIZE_DIRECT_READ );
    wk->setCb([](Fs* fs){
      FsWork *fs_wk = dynamic_cast<FsWork *>(fs);
      shared_ptr<FsWork> wk = fs_wk->shared_from_this();
      auto  res = wk->getRes();
      /**
       * 如果读取失败
       **/ 
      if(fs->req()->result < 0) {
        auto& fl = FileLog::getInstance();
        fl.error("wk->read : " + Util::get_uv_strerror_t(fs->req()->result), __func__, __FILE__, __LINE__);
        res->setHttpStatus(HttpStatus(404));
      }
      /**
       * 读取成功
       **/ 
      else {
        wk->setRead(fs->req()->result);
        res->addMessage(wk->getBuf().base, fs->req()->result);
        if(res->getMode() != TransMode::NORMAL) {
          shared_ptr<HttpResponse> new_res(HttpResponse::newHttpResponse(wk->getReq()->getTcpConnection(), wk->getReq()));
          wk->setRes(new_res);
          /**
           * 设置chunked结束
           **/ 
          if(wk->getRead() >= static_cast<int64_t>(wk->getStatBuf().st_size))
            res->setLastChunked(true);
          else {
            auto fnc = std::bind([](std::shared_ptr<HttpResponse> res, shared_ptr<FsWork> wk, std::shared_ptr<TcpConnection> s){
              static_file_handle_open(wk->getReq(), res, *wk->getWq(), wk);
            }, new_res, wk, std::placeholders::_1);
            res->setAfterWrite(fnc);
          }
          res->end();
        } 
        
      }
      /**
        * 关闭文件
        **/ 
      if(wk->getRead() >= static_cast<int64_t>(wk->getStatBuf().st_size)) {
        wk->setCb([](Fs* ){});
        wk->close(wk->getFile());
      }
    });  
    wk->read(wk->getFile(), static_cast<int64_t>(wk->getRead()));
}

void static_file_handle_open(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, RouteWq &wq, shared_ptr<FsWork> wk)
{
  if(!wk->getRead() && !wk->getFile()) {
    string absolute_path = Util::getRootPath() + Util::conbine_prefix_and_path(req->getCurRe().prefix, req->getRequestPath());
    /**
     * 设置content-type
     **/ 
    res->setContentType(absolute_path);
    wk->setCb([](Fs *fs) {
      FsWork *fs_wk = dynamic_cast<FsWork *>(fs);
      shared_ptr<FsWork> wk = fs_wk->shared_from_this();
      /**
        * 打开文件失败
        **/ 
      if(fs->req()->result < 0) {
        auto& fl = FileLog::getInstance();
        fl.error("wk->open : " + Util::get_uv_strerror_t(fs->req()->result), __func__, __FILE__, __LINE__);
        wk->getRes()->setHttpStatus(HttpStatus(404));
        return;
      }
      /**
       * 打开成功
       **/ 
      wk->setFile(fs->req()->result);
      static_file_handle_read(wk->getReq(), wk->getRes(), *wk->getWq(), wk);
    });
    wk->open(absolute_path.c_str(), O_RDONLY, 0);
  } 
  /**
   * 如果已经打开了
   **/ 
  else {
    static_file_handle_read(wk->getReq(), wk->getRes(), *wk->getWq(), wk);
  }
}

/**
 * 静态文件操作的回调函数，必须由xx::，不然默认是全局的
 **/
void xx::static_file_handle(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, RouteWq &wq)
{
  shared_ptr<FsWork> p = FsWork::newFsWork(req, res, req->getServer());
  p->setWq(&wq);
  /**
   * 第一次进入这个函数先获取文件的信息
   **/
  if (!p->getStatBuf().st_ino && !p->getStatBuf().st_size && !p->getRead())
  {
    string absolute_name = Util::getRootPath() + Util::conbine_prefix_and_path(req->getCurRe().prefix, req->getRequestPath());
    p->setCb([](Fs *req) {
      FsWork *fs_wk = dynamic_cast<FsWork *>(req);
      shared_ptr<FsWork> wk = fs_wk->shared_from_this();
      /**
       * 文件不存在
       **/
      if (req->req()->result)
      {
        auto &fl = FileLog::getInstance();
        fl.debug("stat failed : " + Util::get_uv_strerror_t(req->req()->result), __func__, __FILE__, __LINE__);
        wk->getRes()->setHttpStatus(HttpStatus(404));
        return;
      }
      wk->setStatBuf(&req->req()->statbuf);
      static_file_handle_open(wk->getReq(), wk->getRes(), *wk->getWq(), wk);
    });
    p->stat(absolute_name);
  }
  else
  {
    /**
     * 直接进入对于文件的传输
     **/
    static_file_handle_open(p->getReq(), p->getRes(), *p->getWq(), p);
  }
}

RouteElement::RouteElement(std::regex reg, routeHandleFunc callback,
                           bool is_static, std::string prefix, uint16_t pri, xx::Method m) : reg(reg), callback(callback), is_static(is_static), prefix(prefix), priority(pri), method(m)
{
  if (is_static)
  {
    /**
     * 为什么不推荐形参名字和实参一样，
     * 这里我的意思我想指this->callback
     * 默认却是形参
     **/
    this->callback = static_file_handle;
    priority = RouteElement::FILE_PRIORITY;
  }
}

bool RouteElement::operator<(const RouteElement &ele) const
{
  return this->priority < ele.priority;
}
