#include "net/tcp_session.h"

NAMESPACE_NET

TcpServer::TcpServer(asio::ip::port_type port) :
  port_(port),
  acceptor_(io_context_, { asio::ip::tcp::v4(), port })
{}

void TcpServer::start()
{
  asio::co_spawn(io_context_, accept_connections(), asio::detached);
  io_context_.run();
}

void TcpServer::stop()
{
  sessions_.clear();
  acceptor_.close();
  io_context_.stop();
}

void TcpServer::update()
{
  for (auto it = sessions_.begin(); it != sessions_.end();) {
    it->second->handle_input([this, &it](DataPtr buffer)
    {
      on_receive(it->first, std::move(buffer));
    });

    if (!it->second->connected())
    {
      on_disconnect(it->first);
      it = sessions_.erase(it);
      continue;
    }
    it++;
  }
}

bool TcpServer::running()
{
  return !io_context_.stopped();
}

void TcpServer::send(uint32_t session_id, DataPtr buffer)
{
  if (sessions_.contains(session_id))
    sessions_.at(session_id)->send(std::move(buffer));
}

void TcpServer::send_to_all(DataPtr buffer)
{
  for (auto& session : sessions_)
    session.second->send(buffer);
}

void TcpServer::close_session(uint32_t session_id)
{
  if (sessions_.contains(session_id))
    sessions_.at(session_id)->close();
}

uint32_t TcpServer::use_next_session_id()
{
  return next_session_id_++;
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

TcpClient::TcpClient(const std::string& host, asio::ip::port_type port)
{
  asio::co_spawn(io_context_, [this, host, port]() -> asio::awaitable<void>
  {
    asio::ip::tcp::resolver resolver(io_context_);
    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(host), port);
    asio::ip::tcp::socket asio_socket(io_context_);

    asio::error_code ec;
    co_await asio::async_connect(asio_socket, resolver.resolve(endpoint), asio::redirect_error(asio::use_awaitable, ec));

    auto socket = new_socket(std::move(asio_socket));

    if (!ec)
    {
      session_ = Session::create(0, std::move(socket));
      on_connect();
    }
    else
    {
      connection_failed(ec);
    }
  }, asio::detached);
}

void TcpClient::start()
{
  io_context_.run();
}

void TcpClient::stop()
{
  if (session_)
    session_->close();
  io_context_.stop();
}

void TcpClient::update()
{
  if (!session_)
    return;

  session_->handle_input([this](DataPtr buffer)
  {
    on_receive(std::move(buffer));
  });

  if (!session_->connected())
  {
    on_disconnect();
    session_ = nullptr;
  }
}

bool TcpClient::running()
{
  return !io_context_.stopped();
}

void TcpClient::send(DataPtr buffer)
{
  if (session_)
    session_->send(std::move(buffer));
}

NAMESPACE_NET_END
