#pragma once

#include <memory>
#include <mutex>
#include <queue>

#include <net/common.hpp>
#include <net/socket.hpp>

namespace net
{

class Session final
{
  uint32_t id_;
  std::shared_ptr<Socket> socket_;

  std::mutex input_queue_mutex_;
  std::queue<data_ptr_t> input_queue_;

  std::mutex output_queue_mutex_;
  std::queue<data_ptr_t> output_queue_;
  asio::steady_timer output_signal_;

  Session(const uint32_t id, std::shared_ptr<Socket> socket) :
    id_(id), socket_(std::move(socket)), output_signal_(socket_->get_executor())
  {
    output_signal_.expires_at(std::chrono::steady_clock::time_point::max());
  }

  asio::awaitable<void> write_buffers()
  {
    try
    {
      while (connected())
      {
        {
          std::lock_guard lock(output_queue_mutex_);
          while (!output_queue_.empty())
          {
            co_await socket_->write_buffer(output_queue_.front());
            output_queue_.pop();
          }
        }

        asio::error_code ec;
        co_await output_signal_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
      }
    }
    catch (const std::exception&)
    {
      close();
    }
  }

  asio::awaitable<void> read_buffers()
  {
    try
    {
      while (true)
      {
        std::vector<uint8_t> buffer;
        co_await socket_->read_buffer(buffer);

        std::lock_guard lock(input_queue_mutex_);
        input_queue_.push(std::make_shared<std::vector<uint8_t>>(std::move(buffer)));
      }
    }
    catch (const std::exception&)
    {
      close();
    }
  }

public:
  static std::shared_ptr<Session> create(const uint32_t id, std::shared_ptr<Socket> socket)
  {
    const std::shared_ptr<Session> session(new Session(id, std::move(socket)));
    // start read loop
    asio::co_spawn(
      session->socket_->get_executor(), [session]() -> asio::awaitable<void> { return session->read_buffers(); },
      asio::detached);
    // start write loop
    asio::co_spawn(
      session->socket_->get_executor(), [session]() -> asio::awaitable<void> { return session->write_buffers(); },
      asio::detached);

    return session;
  }

  void close()
  {
    socket_->close();
    output_signal_.cancel();
  }

  [[nodiscard]] bool connected() const { return socket_->connected(); }

  void send(data_ptr_t buffer)
  {
    std::lock_guard lock(output_queue_mutex_);
    output_queue_.push(std::move(buffer));
    output_signal_.cancel_one();
  }

  void handle_input(const std::function<void(data_ptr_t)>& handler)
  {
    std::lock_guard lock(input_queue_mutex_);
    while (!input_queue_.empty())
    {
      handler(input_queue_.front());
      input_queue_.pop();
    }
  }
};

} // namespace net
