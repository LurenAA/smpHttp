#include <iostream>
#include "HttpParser.hpp"

using namespace std;

#define TEST_DATAGRAM \
  "GET /hello.txt HTTP/1.1 \r\n" \
  "User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3 \r\n" \
  "Host: www.example.com \r\n"\
  "Accept-Language: en, mi \r\n\r\n"

int main(int, char**) {
  hpr::HttpParser parser;
  parser.handleDatagram(TEST_DATAGRAM);
}
