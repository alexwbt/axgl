#pragma once

#include <net/client.hpp>
#include <net/common.hpp>

namespace net
{

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

} // namespace net
