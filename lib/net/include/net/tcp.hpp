#pragma once

#include <net/net.hpp>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace net
{

class TcpServer : public Server
{
protected:
  asio::ip::tcp::acceptor acceptor_;

public:
  TcpServer(const std::shared_ptr<asio::io_context>& io_context, const asio::ip::port_type& port) :
    Server(io_context, port), acceptor_(*io_context, {asio::ip::tcp::v4(), port})
  {
  }

  void start() override { asio::co_spawn(*io_context_, accept_connections(), asio::detached); }

  void stop() override
  {
    sessions_.clear();
    acceptor_.close();
  }

  bool running() override { return acceptor_.is_open(); }

  virtual std::shared_ptr<Socket> new_socket(asio::ip::tcp::socket socket) = 0;

private:
  asio::awaitable<void> accept_connections()
  {
    while (running())
    {
      auto asio_socket = co_await acceptor_.async_accept(asio::use_awaitable);
      auto socket = new_socket(std::move(asio_socket));

      const auto session_id = use_next_session_id();
      const auto session = Session::create(session_id, std::move(socket));

      sessions_.insert({session_id, session});
      on_connect(session_id, session);
    }
  }
};

class TcpClient : public Client
{
public:
  using Client::Client;

  void connect(const std::string& host, const asio::ip::port_type& port) override
  {
    asio::co_spawn(
      *io_context_, [this, host, port]() -> asio::awaitable<void>
    {
      asio::error_code ec;
      asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host, ec), port);
      if (ec)
      {
        connection_failed(ec);
        co_return;
      }

      asio::ip::tcp::resolver resolver(*io_context_);
      asio::ip::tcp::socket asio_socket(*io_context_);
      co_await asio::async_connect(
        asio_socket, resolver.resolve(endpoint), asio::redirect_error(asio::use_awaitable, ec));
      if (ec)
      {
        connection_failed(ec);
        co_return;
      }

      auto socket = new_socket(std::move(asio_socket));
      session_ = Session::create(0, std::move(socket));
      on_connect();
    }, asio::detached);
  }

  virtual std::shared_ptr<Socket> new_socket(asio::ip::tcp::socket socket) = 0;
};

class LengthPrefixedTcpSocket final : public Socket
{
  asio::ip::tcp::socket socket_;

public:
  static constexpr size_t kLengthPrefixSize = 4;

  explicit LengthPrefixedTcpSocket(asio::ip::tcp::socket socket) : socket_(std::move(socket)) { }

  asio::awaitable<void> write_buffer(const DataPtr buffer) override
  {
    // prepend size
    const uint32_t size = htonl(static_cast<uint32_t>(buffer->size()));
    const std::array buffers = {
      asio::buffer(&size, kLengthPrefixSize),
      asio::buffer(buffer->data(), buffer->size()),
    };
    co_await asio::async_write(socket_, buffers, asio::use_awaitable);
  }

  asio::awaitable<void> read_buffer(std::vector<uint8_t>& buffer) override
  {
    // read size
    co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, kLengthPrefixSize), asio::use_awaitable);
    const auto size = ntohl(*reinterpret_cast<uint32_t*>(buffer.data()));
    // read all of size
    co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, kLengthPrefixSize + size), asio::use_awaitable);
  }

  void close() override { socket_.close(); }
  bool connected() override { return socket_.is_open(); }
  asio::any_io_executor get_executor() override { return socket_.get_executor(); }
};

} // namespace net
