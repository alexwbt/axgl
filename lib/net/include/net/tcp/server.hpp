#pragma once

#include <net/common.hpp>
#include <net/server.hpp>

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

      sessions_.emplace(session_id, session);
      on_connect(session_id, session);
    }
  }
};

} // namespace net
