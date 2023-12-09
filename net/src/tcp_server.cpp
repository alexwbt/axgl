#include "net/tcp_server.h"
#include <spdlog/spdlog.h>

namespace net
{

  TcpSession::TcpSession(uint32_t id, asio::ip::tcp::socket socket, Handler& handler) :
    id_(id),
    handler_(handler),
    socket_(std::move(socket)),
    timer_(socket_.get_executor())
  {
    asio::co_spawn(socket_.get_executor(), read_buffers(), asio::detached);
    asio::co_spawn(socket_.get_executor(), write_buffers(), asio::detached);
  }

  void TcpSession::send(flatbuffers::DetachedBuffer buffer)
  {
    output_queue_.push(std::move(buffer));
    timer_.cancel_one();
  }

  asio::awaitable<void> TcpSession::write_buffers()
  {
    try
    {
      while (socket_.is_open())
      {
        if (output_queue_.empty())
        {
          asio::error_code ec;
          co_await timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
        }

        const auto& buffer = output_queue_.front();
        co_await asio::async_write(socket_, asio::buffer(buffer.data(), buffer.size()), asio::use_awaitable);
        output_queue_.pop();
      }
    }
    catch (const std::exception& e)
    {
      handler_.disconnect(id_);
    }
  }

  asio::awaitable<void> TcpSession::read_buffers()
  {
    try
    {
      while (true)
      {
        std::vector<uint8_t> buffer;

        // read size
        co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, 4), asio::use_awaitable);

        auto size = flatbuffers::GetSizePrefixedBufferLength(buffer.data());

        // read all of size
        co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, size), asio::use_awaitable);

        std::string identifier(
          flatbuffers::GetBufferIdentifier(buffer.data(), true),
          flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

        handler_.on_receive(id_, identifier, std::move(buffer));
      }
    }
    catch (const std::exception& e)
    {
      handler_.disconnect(id_);
    }
  }

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
    sessions_.clear();
    acceptor_.close();
    io_context_.stop();
  }

  void TcpServer::send(uint32_t session_id, flatbuffers::DetachedBuffer buffer)
  {
    sessions_[session_id]->send(std::move(buffer));
  }

  void TcpServer::disconnect(uint32_t session_id)
  {
    sessions_.erase(session_id);
    on_disconnect(session_id);
  }

  uint32_t TcpServer::use_next_session_id()
  {
    return next_session_id_++;
  }

  asio::awaitable<void> TcpServer::accept_connections()
  {
    while (true)
    {
      auto socket = co_await acceptor_.async_accept(asio::use_awaitable);

      const auto session_id = use_next_session_id();
      const auto session = std::make_shared<TcpSession>(session_id, std::move(socket), *this);

      sessions_.insert({ session_id, session });
      on_connect(session_id, session);
    }
  }

  TcpClient::TcpClient(const std::string& host, asio::ip::port_type port) :
    session_(0, asio::ip::tcp::socket(io_context_), *this)
  {
    asio::ip::tcp::resolver resolver(io_context_);
    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(host), port);
    asio::connect(session_.socket(), resolver.resolve(endpoint));
  }

  void TcpClient::start()
  {
    try
    {
      io_context_.run();
    }
    catch (const std::exception& e)
    {
      SPDLOG_CRITICAL(e.what());
    }
  }

  void TcpClient::send(flatbuffers::DetachedBuffer buffer)
  {
    session_.send(std::move(buffer));
  }

}
