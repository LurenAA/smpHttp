#include "Handle.hpp"

using namespace std;
using namespace uvx;

bool Handle::isClosing() {
  if(!handle) 
    return false;
  return static_cast<bool>(uv_is_closing(handle.get()));
}

Handle::Handle(uv_handle_t* handle,HandleCloseFunc close_cb) :
  handle(handle), close_cb(close_cb)
{
  handle->data = static_cast<void*>(this);
}

Handle::~Handle(){
  this->close();
}

void Handle::close() {
  if(isClosing())
    return;
  uv_close(handle.get(), [](uv_handle_t* handle) {
    Handle* had = static_cast<Handle*>(handle->data);
    had->executeClose_cb();
  });
}

void Handle::setClose_cb(HandleCloseFunc close_cb) {
  this->close_cb = close_cb;
}

void Handle::executeClose_cb() {
  if(close_cb) 
    close_cb();
}