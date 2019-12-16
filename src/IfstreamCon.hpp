// #ifndef __IfstreamCon_HPP
// #define __IfstreamCon_HPP
// #include <fstream>
// #include <string>

// namespace smpHttp {
//   class IfstreamCon
//   {
//   public:
//     using size_type = unsigned long long;
//     std::ifstream& getFs() {return fs;}
//     bool open(std::string path); 
//     void close();
//     bool is_open() { return fs.is_open();}
//     size_type size() {return asize;}
//     // void hasReadNum(size_type x) {has_read += x;}
//     size_type hasRead() {return has_read;}
//     size_type read(std::string& s, size_type max_size);
//     size_type remainNum() { return asize - has_read;}
    
//     ~IfstreamCon();
//   private:
//     std::ifstream fs;
//     std::string path;
//     size_type asize = 0;
//     size_type has_read = 0;
//   };
// }
// #endif //__IfstreamCon_HPP