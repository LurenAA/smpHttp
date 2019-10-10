#include "Handle.hpp"
#include <iostream>

using namespace std;
using namespace uvx;

HandleCloseFunc Handle::close_cb = nullptr;

bool Handle::isClosing() {
  if(!handle) 
    return false;
  return static_cast<bool>(uv_is_closing(handle.get()));
}

Handle::Handle(uv_handle_t* handle) :
  handle(handle)
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
    cout << "log: close one" << endl;
    Handle* had = static_cast<Handle*>(handle->data);
    if(Handle::close_cb)
      had->executeClose_cb();
  });
}

void Handle::setClose_cb(HandleCloseFunc close_cb) {
  Handle::close_cb = close_cb;
}

void Handle::executeClose_cb() {
  if(close_cb) 
    close_cb();
}

Handle::Handle(uv_tcp_t* handle) 
  : Handle(reinterpret_cast<uv_handle_t*>(handle))
{}

// uv_handle_t* Handle::getHandle() {
//   return handle.get();
// }
bool Handle::is_active(){
  return uv_is_active(handle.get());
}
