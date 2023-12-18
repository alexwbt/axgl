#include "net/tcp_server.h"
#include <spdlog/spdlog.h>

namespace net
{

  std::shared_ptr<TcpSession> TcpSession::create(uint32_t id, asio::ip::tcp::socket socket)
  {
    auto session = std::make_shared<TcpSession>(id, std::move(socket));
    // start read loop
    asio::co_spawn(session->socket_.get_executor(),
      [session]() -> asio::awaitable<void> { session->read_buffers(); },
      asio::detached);
    // start write loop
    asio::co_spawn(session->socket_.get_executor(),
      [session]() -> asio::awaitable<void> { session->write_buffers(); },
      asio::detached);
  }

  TcpSession::TcpSession(uint32_t id, asio::ip::tcp::socket socket) :
    id_(id),
    socket_(std::move(socket)),
    timer_(socket_.get_executor())
  {
    timer_.expires_at(std::chrono::steady_clock::time_point::max());
    asio::co_spawn(socket_.get_executor(), read_buffers(), asio::detached);
    asio::co_spawn(socket_.get_executor(), write_buffers(), asio::detached);
  }

  void TcpSession::send(flatbuffers::DetachedBuffer buffer)
  {
    std::lock_guard<std::mutex> lock(output_queue_mutex_);
    output_queue_.push(std::move(buffer));
    timer_.cancel_one();
  }

  void TcpSession::handle_input(std::function<void(std::vector<uint8_t>*)> handler)
  {
    std::lock_guard<std::mutex> lock(input_queue_mutex_);
    while (!input_queue_.empty())
    {
      handler(&input_queue_.front());
      input_queue_.pop();
    }
  }

  void TcpSession::close()
  {
    socket_.close();
    timer_.cancel();
  }

  bool TcpSession::connected()
  {
    return socket_.is_open();
  }

  asio::awaitable<void> TcpSession::write_buffers()
  {
    try
    {
      while (connected())
      {
        {
          std::lock_guard<std::mutex> lock(output_queue_mutex_);
          while (!output_queue_.empty())
          {
            const auto& buffer = output_queue_.front();

            co_await asio::async_write(socket_,
              asio::buffer(buffer.data(), buffer.size()),
              asio::use_awaitable);

            output_queue_.pop();
          }
        }

        asio::error_code ec;
        co_await timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
      }
    }
    catch (const std::exception& e)
    {
      close();
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

        std::lock_guard<std::mutex> lock(input_queue_mutex_);
        input_queue_.push(std::move(buffer));
      }
    }
    catch (const std::exception& e)
    {
      close();
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
    if (sessions_.contains(session_id))
      sessions_.at(session_id)->send(std::move(buffer));
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
      auto socket = co_await acceptor_.async_accept(asio::use_awaitable);

      const auto session_id = use_next_session_id();
      const auto session = TcpSession::create(session_id, std::move(socket));

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
      asio::ip::tcp::socket socket(io_context_);

      asio::error_code ec;
      co_await asio::async_connect(socket, resolver.resolve(endpoint), asio::redirect_error(asio::use_awaitable, ec));

      if (!ec)
      {
        session_ = TcpSession::create(0, std::move(socket));
        on_connect();
      }
      else
        SPDLOG_ERROR("connection failed ({})", ec.message());
    }, asio::detached);
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

  void TcpClient::stop()
  {
    if (session_)
      session_->close();
    io_context_.stop();
  }

  void TcpClient::send(flatbuffers::DetachedBuffer buffer)
  {
    if (session_)
      session_->send(std::move(buffer));
  }

  void TcpClient::disconnect(uint32_t session_id)
  {
    session_ = nullptr;
    on_disconnect();
  }

  void TcpClient::on_receive(uint32_t session_id, const std::string& identifier, std::vector<uint8_t> buffer)
  {
    on_receive(identifier, buffer);
  }

}
