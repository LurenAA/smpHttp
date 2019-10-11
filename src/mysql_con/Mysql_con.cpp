#include "Mysql_con.hpp"
#include <iostream>

using namespace ::mysqlx;
using namespace std;

Mysql_con::Mysql_con(const std::string& host, const std::string &user,const std::string& db, const char* pwd, unsigned port) try:
  session(host, port, user, pwd, db), host(host), user(user), pwd(pwd), db(db), port(port)
{
} catch(Error e) {
  cout << e.what() << endl;
}

Mysql_con::~Mysql_con() {
  session.close();
}

void Mysql_con::addSchema(const std::string& s) {
  try {
    Schema sch = session.getSchema(s, true);
    map_sch.insert({s, sch});
  } catch(Error e) {
    cout << e.what() << endl;
  }
}

Table Mysql_con::getTable(const std::string& schema, const std::string& table) {  
  auto fnd = map_sch.find(schema);
  if(fnd == map_sch.end())
    addSchema(schema);
  Schema& sch = map_sch.at(schema);
  return sch.getTable(table, true);
}