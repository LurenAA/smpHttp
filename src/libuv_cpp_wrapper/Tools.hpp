#ifndef __TOOLS__H__
#define __TOOLS__H__
#include <string>
#include "Mutex.hpp"

namespace xx {
class Tools {
public:
  static Tools& getInstance();

  std::string get_strerror_r(int errno_r);
  std::string get_uv_strerror_t(int errno_r);

  Tools& operator=(const Tools&) = delete;
  Tools(const Tools&) = delete;
private:
  Tools() = default;
  static Tools* singleton;
  static Mutex mx, mx_sr, mx_uv_sr;
};
}

#endif