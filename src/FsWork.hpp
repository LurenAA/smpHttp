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
      HttpServer & getServer();
    private:
      explicit FsWork(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res,HttpServer &_server);
      struct stat stat_buf;

      std::shared_ptr<HttpRequest> req;
      std::shared_ptr<HttpResponse> res;
      HttpServer &_server;
  };
} // namespace xx

#endif