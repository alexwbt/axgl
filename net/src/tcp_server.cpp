#include "net/tcp_server.h"

#include <asio/detached.hpp>
#include <asio/co_spawn.hpp>
#include <asio/awaitable.hpp>
#include <asio/use_awaitable.hpp>

namespace net {

  asio::awaitable<void> _start(
    TcpServer* server,
    asio::io_context& io_context,
    asio::ip::port_type port)
  {
    asio::ip::tcp::acceptor acceptor(io_context, { asio::ip::tcp::v4(), port });
    while (true)
    {
      auto socket = co_await acceptor.async_accept(asio::use_awaitable);
      server->on_new_connection(std::move(socket));
    }
  }

  void TcpServer::start(asio::io_context& io_context, asio::ip::port_type port)
  {
    asio::co_spawn(io_context,
      _start(this, io_context, port),
      asio::detached);
  }

}
