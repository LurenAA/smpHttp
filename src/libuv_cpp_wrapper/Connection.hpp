// #ifndef _CONNECTION123_HPP_
// #define _CONNECTION123_HPP_
// #include "uv.h"
// #include <memory>
// #include "Handle.hpp"
// #include <functional>
// #include <string>
// #include <cstring>
// #include "EventLoop.hpp"

// namespace uvx {
// class Connection;
// using ReadCallback = std::function<void(std::shared_ptr<Connection>)>;
// using WriteCallback = std::function<void()>;
// using StartRedFunc_t = std::function<void (uv_stream_t *, ssize_t , const uv_buf_t *)>;
// class Tcp;

// class Connection : public Handle, public std::enable_shared_from_this<Connection>{
// public:

//   Connection(uv_tcp_t* handle, Tcp* tcp);
//   void startRead();
//   void write(const char* str, std::string::size_type len) ;
//   void write(std::string str);
//   uv_tcp_t* getHandle();
//   ReadCallback setReadCallback(ReadCallback);
//   WriteCallback setWriteCallback(WriteCallback);
//   WriteCallback getWriteCallback() const {return writeCallback;}
//   void onRead(std::shared_ptr<uvx::Connection>);
//   void onWrite();
//   bool is_writable() {return uv_is_writable(reinterpret_cast<const uv_stream_t*>(handle.get()));}
//   int get_send_buf_size() const ;
//   void set_send_buf_size(int s);
//   uvx::Loop& _loop();

// private:
//   Tcp* tcp;
  
//   ReadCallback readCallback = nullptr;
//   WriteCallback writeCallback = nullptr;
//   virtual void onStartRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf){};
//   void onClose() override;

//   static void allocFunc(uv_handle_t*, size_t, uv_buf_t*);
// };

// struct ReqEntity {
//   uv_write_t req;
//   std::string reserve = "";
//   uv_buf_t buf;
//   std::shared_ptr<Connection> cl;
//   std::string rest_string = "";
//   void init() {
//     ::memset(&req, 0, sizeof(req));
//     buf.base = const_cast<char*>(reserve.c_str());
//     buf.len = reserve.size();
//     req.data = this;
//   }
//   void setReserve(const char * str, size_t len) {
//     reserve.clear();
//     reserve.assign(str, len);
//   }
//   void setReserve(const std::string &s) {
//     setReserve(s.c_str(), s.size());
//   }
// };
// }

// #endif //CONNECTION_HPP_