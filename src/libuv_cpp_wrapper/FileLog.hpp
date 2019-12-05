#ifndef __LOG_HPP_
#define __LOG_HPP_
#include <string>
#include "log4cpp/Category.hh"
#include "log4cpp/PropertyConfigurator.hh"

namespace xx {
  class FileLog : public log4cpp::Category{
    public:
      static void init(const std::string& init_path = "./file_log.ini");
  };
}

#endif