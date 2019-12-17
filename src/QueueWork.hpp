#ifndef __QUEUE_WORK_HPP
#define __QUEUE_WORK_HPP
#include "CCommon.hpp"
#include <exception>
#include <string>
#include "FileLog.hpp"
#include "Util.hpp"

namespace xx
{
class HttpServer;
class HttpRequest;
class HttpResponse;
class QueueWork : public std::enable_shared_from_this<QueueWork>
{
public:
  static std::shared_ptr<QueueWork> newQueueWork(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res,
                                                 HttpServer &_server);

  QueueWork(const QueueWork &) = delete;
  QueueWork &operator=(const QueueWork &) = delete;
  bool remove();
  bool work();

  template <typename F, typename... A>
  bool setWorkCb(F f, A... args);

  template <typename F, typename... A>
  bool setAfterWorkCb(F f, A... args);
  std::shared_ptr<HttpRequest> getReq() const;
  std::shared_ptr<HttpResponse> getRes() const;
  HttpServer &getServer() const;

  void invokeWorkCb();
  void invokeAfterWorkCb(int);

private:
  QueueWork(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res, HttpServer &_server);

  std::shared_ptr<HttpRequest> req;
  std::shared_ptr<HttpResponse> res;
  HttpServer &_server;
  uv_work_t wk;

  QueueWorkCbType work_cb = nullptr;            //工作函数
  QueueAfterWorkCbType after_work_cb = nullptr; //工作函数的回调函数
};

/**
 * 注册工作函数的回调函数
 **/
template <typename F, typename... A>
bool QueueWork::setAfterWorkCb(F f, A... args)
{
  try
  {
    if (!sizeof...(args))
    {
      after_work_cb = f;
    }
    else
    {
      QueueAfterWorkCbType func = std::bind(f, std::placeholders::_1, std::placeholders::_2, args...);
      after_work_cb = func;
    }
  }
  catch (std::exception &e)
  {
    FileLog &fl = FileLog::getInstance();
    fl.debug(std::string("QueueWork::after_work_cb: ") + e.what(), __func__,
             __FILE__, __LINE__);
    return false;
  }
  return true;
}

/**
 * 注册工作函数
 **/
template <typename F, typename... A>
bool QueueWork::setWorkCb(F f, A... args)
{
  try
  {
    if (sizeof...(args) == 0)
      work_cb = f;
    else
    {
      auto func = std::bind(f, std::placeholders::_1, args...);
      work_cb = func;
    }
  }
  catch (std::exception &e)
  {
    FileLog &fl = FileLog::getInstance();
    fl.debug(std::string("QueueWork::rejister_work_cb : ") + e.what(), __func__,
             __FILE__, __LINE__);
    return false;
  }
  return true;
}
} // namespace xx

#endif