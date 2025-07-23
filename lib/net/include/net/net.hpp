#pragma once

#include <unordered_map>
#include <ranges>
#include <memory>
#include <mutex>
#include <queue>

#ifdef _WIN32
#include <SDKDDKVer.h>
#endif

#include <asio.hpp>

namespace net
{

typedef const std::vector<uint8_t> DataContainer;
typedef std::shared_ptr<DataContainer> DataPtr;

class Socket
{
public:
  virtual ~Socket() = default;

  virtual asio::awaitable<void> write_buffer(DataPtr buffer) = 0;
  virtual asio::awaitable<void> read_buffer(std::vector<uint8_t>& buffer) = 0;

  virtual void close() = 0;
  virtual bool connected() = 0;
  virtual asio::any_io_executor get_executor() = 0;
};

class Session final
{
  uint32_t id_;
  std::shared_ptr<Socket> socket_;

  std::mutex input_queue_mutex_;
  std::queue<DataPtr> input_queue_;

  std::mutex output_queue_mutex_;
  std::queue<DataPtr> output_queue_;
  asio::steady_timer output_signal_;

  Session(const uint32_t id, std::shared_ptr<Socket> socket) :
    id_(id),
    socket_(std::move(socket)),
    output_signal_(socket_->get_executor())
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
    asio::co_spawn(session->socket_->get_executor(),
      [session]() -> asio::awaitable<void> { return session->read_buffers(); },
      asio::detached);
    // start write loop
    asio::co_spawn(session->socket_->get_executor(),
      [session]() -> asio::awaitable<void> { return session->write_buffers(); },
      asio::detached);

    return session;
  }

  void close()
  {
    socket_->close();
    output_signal_.cancel();
  }

  [[nodiscard]] bool connected() const
  {
    return socket_->connected();
  }

  void send(DataPtr buffer)
  {
    std::lock_guard lock(output_queue_mutex_);
    output_queue_.push(std::move(buffer));
    output_signal_.cancel_one();
  }

  void handle_input(const std::function<void(DataPtr)>& handler)
  {
    std::lock_guard lock(input_queue_mutex_);
    while (!input_queue_.empty())
    {
      handler(input_queue_.front());
      input_queue_.pop();
    }
  }
};

class Server
{
  uint32_t next_session_id_ = 1;

protected:
  std::shared_ptr<asio::io_context> io_context_;

  const asio::ip::port_type port_;
  std::unordered_map<uint32_t, std::shared_ptr<Session>> sessions_;

public:
  virtual ~Server() = default;

  Server(std::shared_ptr<asio::io_context> io_context, const asio::ip::port_type& port) :
    io_context_(std::move(io_context)), port_(port)
  {}

  virtual void update()
  {
    for (auto it = sessions_.begin(); it != sessions_.end();)
    {
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
      ++it;
    }
  }

  virtual void send(const uint32_t session_id, DataPtr buffer)
  {
    if (sessions_.contains(session_id))
      sessions_.at(session_id)->send(std::move(buffer));
  }

  virtual void send_to_all(DataPtr buffer)
  {
    for (const auto& val : sessions_ | std::views::values)
      val->send(buffer);
  }

  virtual void close_session(const uint32_t session_id)
  {
    if (sessions_.contains(session_id))
      sessions_.at(session_id)->close();
  }

  virtual void on_disconnect(uint32_t session_id) {}
  virtual void on_receive(uint32_t session_id, DataPtr buffer) {}
  virtual void on_connect(uint32_t session_id, std::shared_ptr<Session> session) {}

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual bool running() = 0;

protected:
  uint32_t use_next_session_id() { return next_session_id_++; }
};

class Client
{
protected:
  std::shared_ptr<Session> session_;
  std::shared_ptr<asio::io_context> io_context_;

public:
  virtual ~Client() = default;

  explicit Client(std::shared_ptr<asio::io_context> io_context) :
    io_context_(std::move(io_context))
  {}

  virtual void disconnect()
  {
    if (session_)
      session_->close();
  }

  virtual void update()
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

  virtual bool connected()
  {
    return session_ && session_->connected();
  }

  virtual void send(DataPtr buffer)
  {
    if (session_)
      session_->send(std::move(buffer));
  }

  virtual void on_connect() {}
  virtual void on_disconnect() {}
  virtual void on_receive(DataPtr buffer) {}
  virtual void connection_failed(const asio::error_code& error_code) {}

  virtual void connect(const std::string& host, const asio::ip::port_type& port) = 0;
};

}
