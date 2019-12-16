// #include "IfstreamCon.hpp"
// #include <iostream>
// #include "unistd.h"

// using namespace std;
// using namespace smpHttp;

// bool IfstreamCon::open(std::string apath){
//   if(apath.size()) {
//     path = apath;
//     fs.open(apath, ios::binary);
//     bool status = is_open();
//     if(status) {
//       fs.seekg(0, ios::end);
//       asize = fs.tellg();
//       fs.seekg(0, ios::beg);
//       return true;
//     }
//     return false; 
//   }
//   return false;
// }

// IfstreamCon::~IfstreamCon() {
//   close();
// }

// void IfstreamCon::close() {
//   if(is_open()) 
//     fs.close();
// }

// IfstreamCon::size_type IfstreamCon::read(std::string& s, size_type max_size){
//   char buf[max_size + 1] = ""; //the additional one space is very important
//   size_type remain = asize - has_read;
//   // cout << remain << endl;
//   bool key = true; 
//   if(remain <= max_size) {
//     key = false;
//   } 
//   std::streamsize si = key ? max_size : remain;
//   fs.read(buf, si);
//   buf[si] = '\0';
//   has_read += si;
//   s.assign(buf, si);
//   return si;
// }