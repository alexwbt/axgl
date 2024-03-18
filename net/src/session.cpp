#include "net/session.h"

// #include <spdlog/spdlog.h>
// #include <flatbuffers/flatbuffers.h>

namespace net
{

  std::shared_ptr<Session> Session::create(uint32_t id, std::shared_ptr<Socket> socket)
  {
    auto session = std::make_shared<Session>(id, std::move(socket));
    // start read loop
    asio::co_spawn(session->socket_->get_executor(),
      [session]() -> asio::awaitable<void> { return session->read_buffers(); },
      asio::detached);
    // start write loop
    asio::co_spawn(session->socket_->get_executor(),
      [session]() -> asio::awaitable<void> { return session->write_buffers(); },
      asio::detached);

    return session;
  }

  Session::Session(uint32_t id, std::shared_ptr<Socket> socket) :
    id_(id),
    socket_(std::move(socket)),
    output_signal_(socket_->get_executor())
  {
    output_signal_.expires_at(std::chrono::steady_clock::time_point::max());
  }

  void Session::send(DataPtr buffer)
  {
    std::lock_guard<std::mutex> lock(output_queue_mutex_);
    output_queue_.push(std::move(buffer));
    output_signal_.cancel_one();
  }

  void Session::handle_input(std::function<void(DataPtr)> handler)
  {
    std::lock_guard<std::mutex> lock(input_queue_mutex_);
    while (!input_queue_.empty())
    {
      handler(input_queue_.front());
      input_queue_.pop();
    }
  }

  void Session::close()
  {
    socket_->close();
    output_signal_.cancel();
  }

  bool Session::connected()
  {
    return socket_->connected();
  }

  asio::awaitable<void> Session::write_buffers()
  {
    try
    {
      while (connected())
      {
        {
          std::lock_guard<std::mutex> lock(output_queue_mutex_);
          while (!output_queue_.empty())
          {
            // const auto buffer = output_queue_.front();

            // co_await asio::async_write(socket_,
            //   asio::buffer(buffer->data(), buffer->size()),
            //   asio::use_awaitable);
            socket_->write_buffer(output_queue_.front());
            output_queue_.pop();
          }
        }

        asio::error_code ec;
        co_await output_signal_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
      }
    }
    catch (const std::exception& e)
    {
      close();
    }
  }

  asio::awaitable<void> Session::read_buffers()
  {
    try
    {
      while (true)
      {
        std::vector<uint8_t> buffer;
        socket_->read_buffer();

        // read size
        // co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, 4), asio::use_awaitable);

        // auto size = flatbuffers::GetSizePrefixedBufferLength(buffer.data());

        // read all of size
        // co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, size), asio::use_awaitable);

        std::lock_guard<std::mutex> lock(input_queue_mutex_);
        input_queue_.push(std::make_shared<std::vector<uint8_t>>(std::move(buffer)));
      }
    }
    catch (const std::exception& e)
    {
      close();
    }
  }

}
