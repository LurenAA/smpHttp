#include "FileLog.hpp"
#include "pthread.h"
#include <functional>
#include <cstring>
using namespace std;
using namespace xx;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static std::string init_path = "";

static void reset_init_once() {
  pthread_once_t child_once = PTHREAD_ONCE_INIT;
  ::memcpy(&init_once, &child_once, sizeof(child_once));
}

static void _init() {
  if(pthread_atfork(nullptr, nullptr, &reset_init_once)) {
    cout << "fatal: pthread_atfork error" << endl;
    terminate();
  }
  try
  {
    if(init_path.empty()) 
      return ;
    log4cpp::PropertyConfigurator::configure(init_path);
  }
  catch(log4cpp::ConfigureFailure& f)
  {
    std::cout<< "Configure Problem : "<< f.what()<< std::endl;
    terminate();
  }
}

void FileLog::init(const std::string& ainit_path) {
  init_path = ainit_path;
  if(pthread_once(&init_once, _init))
    terminate();
}
