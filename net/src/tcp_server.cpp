#include "net/tcp_server.h"
#include <spdlog/spdlog.h>

namespace net
{

  TcpServer::TcpServer(asio::ip::port_type port) :
    port_(port),
    acceptor_(io_context_, { asio::ip::tcp::v4(), port })
  {}

  void TcpServer::start()
  {
    try
    {
      SPDLOG_INFO("starting server on {}", port_);

      asio::co_spawn(io_context_, accept_connections(), asio::detached);
      io_context_.run();

      SPDLOG_INFO("server stopped");
    }
    catch (const std::exception& e)
    {
      SPDLOG_CRITICAL(e.what());
    }
  }

  void TcpServer::stop()
  {
    sockets_.clear();
    acceptor_.close();
    io_context_.stop();
  }

  void TcpServer::remove_socket(uint32_t id)
  {
    sockets_.erase(id);
  }

  uint32_t TcpServer::use_next_socket_id()
  {
    return next_socket_id_++;
  }

  asio::awaitable<void> TcpServer::accept_connections()
  {
    while (true)
    {
      const auto socket = std::make_shared<asio::ip::tcp::socket>(
        co_await acceptor_.async_accept(asio::use_awaitable));
      const auto socket_id = use_next_socket_id();

      sockets_.insert({ socket_id, socket });
      on_new_connection(socket_id, socket);
    }
  }

}
