#include "net/tcp_session.h"

NAMESPACE_NET

/* TcpServer */

TcpServer::TcpServer(std::shared_ptr<asio::io_context> io_context, const asio::ip::port_type& port) :
  Server(io_context, port),
  acceptor_(*io_context, { asio::ip::tcp::v4(), port })
{}

void TcpServer::start()
{
  asio::co_spawn(*io_context_, accept_connections(), asio::detached);
}

void TcpServer::stop()
{
  sessions_.clear();
  acceptor_.close();
}

bool TcpServer::running()
{
  return acceptor_.is_open();
}

asio::awaitable<void> TcpServer::accept_connections()
{
  while (true)
  {
    auto asio_socket = co_await acceptor_.async_accept(asio::use_awaitable);
    auto socket = new_socket(std::move(asio_socket));

    const auto session_id = use_next_session_id();
    const auto session = Session::create(session_id, std::move(socket));

    sessions_.insert({ session_id, session });
    on_connect(session_id, session);
  }
}

/* TcpClient */

void TcpClient::connect(const std::string& host, const asio::ip::port_type& port)
{
  asio::co_spawn(*io_context_, [this, host, port]() -> asio::awaitable<void>
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
    co_await asio::async_connect(asio_socket, resolver.resolve(endpoint), asio::redirect_error(asio::use_awaitable, ec));
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

NAMESPACE_NET_END
