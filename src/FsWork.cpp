#include "FsWork.hpp"
#include "HttpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

using namespace std;
using namespace xx;

FsWork::FsWork(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res,HttpServer &_server) 
  : Fs(*(_server.getLoop())) , req(req), res(res), _server(_server)
{
  this->_callback = [](uv_fs_t *req1) {
    Fs *fs = reinterpret_cast<Fs *>(req1->data);
    fs->setState(FsState::idle);
    if(!fs) 
      return ;
    FsWork* fs_w = dynamic_cast<FsWork*>(fs);
    shared_ptr<FsWork> fp = fs_w->shared_from_this();
    /**
     * 移除队列，要用到shared_from_this()所以必须由共享指针发起调用
     **/ 
    fp->remove();
    fs->invokeCb();
  };
}

std::shared_ptr<HttpRequest> FsWork::getReq() const{
  return req;
}

std::shared_ptr<HttpResponse> FsWork::getRes() const{
  return res;
}

HttpServer & FsWork::getServer() {
  return _server;
}

/**
 * 限制只能获得共享指针的对象
 **/ 
std::shared_ptr<FsWork> FsWork::newFsWork(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res,HttpServer &_server)
{
  shared_ptr<FsWork> resq(new FsWork(req, res, _server));
  return resq;
}

bool FsWork::remove() {
  return this->_server.remove_fs_work(shared_from_this());
}

int FsWork::open(const std::string& path, int flags, int mode) {
  _server.add_fs_work(shared_from_this());
  return Fs::open(path, flags, mode);
}

int FsWork::close(uv_file file) {
  _server.add_fs_work(shared_from_this());
  return Fs::close(file);
}

int FsWork::read(uv_file file, int64_t offset) {
  _server.add_fs_work(shared_from_this());
  return Fs::read(file, offset);
}

int FsWork::write(uv_file file, int64_t offset) {
  _server.add_fs_work(shared_from_this());
  return Fs::write(file, offset);
}

int FsWork::stat(const std::string& s) {
  _server.add_fs_work(shared_from_this());
  return Fs::stat(s);
}

