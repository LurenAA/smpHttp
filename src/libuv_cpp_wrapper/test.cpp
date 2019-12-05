#include <iostream>
#include "FileLog.hpp"
using namespace std;
using namespace xx;

int main(void) {
  FileLog::init();
  auto& r  = FileLog::getRoot();
  r.debug("abc");
  r.error("asd");
  FileLog::shutdown();
  return 0; 
}