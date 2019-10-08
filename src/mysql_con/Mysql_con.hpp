#ifndef __MYSQL_HPP__
#define __MYSQL_HPP__
#include "mysqlx/xdevapi.h"
#include <string>
#include <map>

namespace mysqlx{
  class Mysql_con {
    public:
      explicit Mysql_con(const std::string&, const std::string &user, 
        const char* pwd=nullptr, unsigned port = 33060);
      Mysql_con(const Mysql_con&) = delete;
      Mysql_con& operator=(const Mysql_con&) = delete;
      ~Mysql_con();
      void addSchema(const std::string&);
      Table getTable(const std::string&, const std::string&);
    private:
      Session session;
      std::map<std::string, Schema> map_sch;
      std::string host, user, pwd, db;
      unsigned int port;
  };
}

#endif //__MYSQL_HPP__