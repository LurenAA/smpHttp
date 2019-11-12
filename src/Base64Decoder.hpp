#ifndef __BASE64_DECODE
#define __BASE64_DECODE
#include <string>

namespace smpHttp
{
  class Base64Decoder {
    public:
      static std::string base64_decode(const char* s);
      static std::string* base64_decode(const char* s, bool return_malloc);
    private: 
      static const std::string base64_chars;
      static const char* base64_decode_chars;
  };
} // namespace smpHttp


#endif