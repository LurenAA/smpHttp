#include "Base64Decoder.hpp"
#include <iostream>
#include <cstring>
using namespace std;
using namespace smpHttp;


const std::string Base64Decoder::base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
const char* Base64Decoder::base64_decode_chars = ([](){
  char *res = new char[256]();
  for(std::string::size_type x = 0; x < base64_chars.size(); ++x) {
    res[static_cast<std::string::size_type>(base64_chars[x])] = x;
  }
  return res;
})();


std::string Base64Decoder::base64_decode(const char* s) {
  std::string res;
  char buf_3[3] = "",
    buf_4[4] = "";
  size_t max_size = ::strlen(s);
  size_t i = 0, count_j = 0;
  for(;i < max_size; ++i) {
    ++count_j;
    if(count_j == 4) {
      buf_4[count_j - 1] = s[i];
      count_j = 0;
      //handle
      buf_3[0] = 0xff & (base64_decode_chars[static_cast<std::string::size_type>(buf_4[0])] << 2 | base64_decode_chars[static_cast<std::string::size_type>(buf_4[1])] >> 4);
      buf_3[1] = 0xff & (base64_decode_chars[static_cast<std::string::size_type>(buf_4[1])] << 4 | base64_decode_chars[static_cast<std::string::size_type>(buf_4[2])] >> 2);
      buf_3[2] = 0xff & (base64_decode_chars[static_cast<std::string::size_type>(buf_4[2])] << 6 | base64_decode_chars[static_cast<std::string::size_type>(buf_4[3])]);
      int arg_c = 0;
      for(int j = 3; j >= 0; --j) {
        if(buf_4[j] == '=')
          arg_c++;
      }
      for(int j = 0;j < 3 - arg_c;j++) 
        res += buf_3[j];
    } else {
      buf_4[count_j - 1] = s[i];
    }
  }
  return res;
}