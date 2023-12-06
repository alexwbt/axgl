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
    asio::co_spawn(socket_.get_executor(), read_messages(), asio::detached);
    asio::co_spawn(socket_.get_executor(), write_messages(), asio::detached);
  }

  TcpServer::Session::~Session()
  {
    server_.on_disconnect(id_);
  }

  void TcpServer::Session::send_message(const std::string& message)
  {
    output_queue_.push(message);
    timer_.cancel_one();
  }

  asio::awaitable<void> TcpServer::Session::write_messages()
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
        co_await asio::async_write(socket_,
          asio::buffer(output_queue_.front()), asio::use_awaitable);
        output_queue_.pop();
      }
    }
    catch (const std::exception& e)
    {
      server_.remove_session(id_);
    }
  }

  asio::awaitable<void> TcpServer::Session::read_messages()
  {
    try
    {
      std::string message;
      while (true)
      {
        std::size_t n = co_await asio::async_read_until(socket_,
          asio::dynamic_buffer(message, 1024), "\n", asio::use_awaitable);

        server_.on_receive(id_, message);
        message.erase(0, n);
      }
    }
    catch (const std::exception& e)
    {
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

  void TcpServer::remove_session(uint32_t id)
  {
    sessions_.erase(id);
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
