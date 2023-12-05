#include "net/tcp_server.h"

namespace net {

  TcpServer::TcpServer(asio::ip::port_type port) :
    acceptor_(io_context_, { asio::ip::tcp::v4(), port })
  {}

  void TcpServer::start()
  {
    async_run([this]() -> asio::awaitable<void> {
      while (true)
      {
        auto socket = co_await acceptor_.async_accept(asio::use_awaitable);
        on_new_connection(std::move(socket));
      }
    }());
    io_context_.run();
  }

}
