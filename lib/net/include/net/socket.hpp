#pragma once

#include <net/common.hpp>

namespace net
{

class Socket
{
public:
  virtual ~Socket() = default;

  virtual asio::awaitable<void> write_buffer(data_ptr_t buffer) = 0;
  virtual asio::awaitable<void> read_buffer(std::vector<uint8_t>& buffer) = 0;

  virtual void close() = 0;
  virtual bool connected() = 0;
  virtual asio::any_io_executor get_executor() = 0;
};

} // namespace net
