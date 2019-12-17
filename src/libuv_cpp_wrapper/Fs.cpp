#include "Fs.hpp"
#include <iostream>
#include "EventLoop.hpp"

using namespace std;
using namespace xx;

void Fs::clean_up()
{
  uv_fs_req_cleanup(req());
}

Fs::Fs(EventLoop &_lp) :_lp(_lp), cb(nullptr)
{
  _fs.data = this;
}

uv_fs_t *Fs::req()
{
  return &_fs;
}

/**
 * open
 **/
int Fs::open(const std::string &path, int flags, int mode)
{
  int flag = uv_fs_open(_lp.handle(), req(), path.c_str(), flags, mode, [](uv_fs_t *req1) 
  {
    Fs *fs = reinterpret_cast<Fs *>(req1->data);
    if(!fs) 
      return ;
    fs->invokeCb();
  });
}

void Fs::setCb(FsType f)
{
  cb = f;
}

void Fs::invokeCb()
{
  if (this->cb)
  {
    cb(this);
  }
}