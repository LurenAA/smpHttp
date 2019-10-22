#include "Handle.hpp"
#include <iostream>
#include <cassert>

using namespace std;
using namespace uvx;

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
  assert(isClosing());
}

void Handle::close() {
  if(isClosing())
    return;
  uv_close(handle.get(), [](uv_handle_t* handle) {
    cout << "log: close a handle" << endl;
    Handle* had = static_cast<Handle*>(handle->data);
    had->onClose();
  });
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
