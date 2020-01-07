#ifndef __FS_WORK_HPP
#define __FS_WORK_HPP
#include "CCommon.hpp"
#include <exception>
#include <string>
#include "FileLog.hpp"
#include "Util.hpp"
#include "Fs.hpp"
#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace xx
{
  class HttpServer;
  class HttpRequest;
  class HttpResponse;
  class RouteWq;
  class FsWork : public Fs , public std::enable_shared_from_this<FsWork>{
    public:
      static std::shared_ptr<FsWork> newFsWork(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res,HttpServer &_server);

      bool remove();
      int open(const std::string& path, int flags, int mode) override;
      int close(uv_file file) override;
      int read(uv_file file, int64_t offset) override;
      int write(uv_file file, int64_t offset) override;
      int stat(const std::string&) override;
      
      std::shared_ptr<HttpRequest> getReq() const;
      std::shared_ptr<HttpResponse> getRes() const;
      void setRes(std::shared_ptr<HttpResponse>);

      HttpServer & getServer();
      int64_t getMaxSizeDirectRead() const;
      void setMaxSizeDirectRead(int64_t);
      void setStatBuf(uv_stat_t*);
      const uv_stat_t& getStatBuf() const;
      void setWq(RouteWq* );
      RouteWq* getWq() const;
      int64_t getRead() const;
      void setRead(int64_t);
      uv_file getFile() const;
      void setFile(uv_file);
    private:
      explicit FsWork(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res,HttpServer &_server);
      uv_stat_t stat_buf; //fs_stat
      int64_t max_size_direct_read = DEFAULT_MAX_SIZE_DIRECT_READ;
      RouteWq *wq; //让文件操作回调函数中还能回到队列中的回调函数
      int64_t _read = 0;//记录已经读取的字节数
      uv_file file = 0; //记录开启的文件描述符

      std::shared_ptr<HttpRequest> req;
      std::shared_ptr<HttpResponse> res;
      HttpServer &_server;
  };
} // namespace xx

#endif