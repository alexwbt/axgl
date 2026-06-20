#pragma once

#include <cstdint>
#include <net/common.hpp>
#include <net/socket.hpp>

#ifdef _WIN32
  #include <winsock2.h>
#else
  #include <arpa/inet.h>
#endif

namespace net
{

class LengthPrefixedTcpSocket final : public Socket
{
  asio::ip::tcp::socket socket_;

public:
  static constexpr size_t kLengthPrefixSize = 4;

  explicit LengthPrefixedTcpSocket(asio::ip::tcp::socket socket) : socket_(std::move(socket)) { }

  asio::awaitable<void> write_buffer(const data_ptr_t buffer) override
  {
    // prepend size
    const std::uint32_t size = htonl(static_cast<std::uint32_t>(buffer->size()));
    const std::array buffers = {
      asio::buffer(&size, kLengthPrefixSize),
      asio::buffer(buffer->data(), buffer->size()),
    };
    co_await asio::async_write(socket_, buffers, asio::use_awaitable);
  }

  asio::awaitable<void> read_buffer(std::vector<std::uint8_t>& buffer) override
  {
    // read size
    co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, kLengthPrefixSize), asio::use_awaitable);
    const auto size = ntohl(*reinterpret_cast<std::uint32_t*>(buffer.data()));
    // read all of size
    co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, kLengthPrefixSize + size), asio::use_awaitable);
  }

  void close() override { socket_.close(); }
  bool connected() override { return socket_.is_open(); }
  asio::any_io_executor get_executor() override { return socket_.get_executor(); }
};

} // namespace net
