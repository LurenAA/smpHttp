#ifndef __TOOLS__H__
#define __TOOLS__H__

namespace xx {
class Tools {
  public:
  Tools& getInstance();
  Tools& operator=(const Tools&) = delete;
  Tools(const Tools&) = delete;
  ~Tools();
  private:
  static Tools* singleton;
};
}

#endif