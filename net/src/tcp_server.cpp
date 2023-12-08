#include "net/tcp_server.h"
#include <spdlog/spdlog.h>

namespace net
{

  TcpServer::Session::Session(TcpServer& server, uint32_t id, asio::ip::tcp::socket socket) :
    server_(server),
    id_(id),
    socket_(std::move(socket)),
    timer_(socket_.get_executor())
  {
    asio::co_spawn(socket_.get_executor(), read_buffers(), asio::detached);
    asio::co_spawn(socket_.get_executor(), write_buffers(), asio::detached);
  }

  TcpServer::Session::~Session()
  {
    server_.on_disconnect(id_);
  }

  void TcpServer::Session::send(flatbuffers::DetachedBuffer buffer)
  {
    output_queue_.push(std::move(buffer));
    timer_.cancel_one();
  }

  asio::awaitable<void> TcpServer::Session::write_buffers()
  {
    try
    {
      if (output_queue_.empty())
      {
        asio::error_code ec;
        co_await timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
      }
      else
      {
        const auto& buffer = output_queue_.front();
        co_await asio::async_write(socket_, asio::buffer(buffer.data(), buffer.size()), asio::use_awaitable);
        output_queue_.pop();
      }
    }
    catch (const std::exception& e)
    {
      server_.remove_session(id_);
    }
  }

  asio::awaitable<void> TcpServer::Session::read_buffers()
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

        server_.on_receive(id_, identifier, std::move(buffer));
      }
    }
    catch (const std::exception& e)
    {
      // disconnect
      server_.remove_session(id_);
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

  void TcpServer::remove_session(uint32_t session_id)
  {
    sessions_.erase(session_id);
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
      const auto session = std::make_shared<Session>(*this, session_id, std::move(socket));

      sessions_.insert({ session_id, session });
      on_connect(session_id, session);
    }
  }

}
