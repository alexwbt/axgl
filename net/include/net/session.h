#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

#include "net/asio.h"
#include "net/namespace.h"

NAMESPACE_NET

typedef std::shared_ptr<std::vector<uint8_t>> DataPtr;

class Socket
{
public:
  virtual ~Socket() {}

  virtual asio::awaitable<void> write_buffer(DataPtr buffer) = 0;
  virtual asio::awaitable<void> read_buffer(std::vector<uint8_t>& buffer) = 0;

  virtual void close() = 0;
  virtual bool connected() = 0;

  virtual asio::any_io_executor get_executor() = 0;
};

class Session final
{
private:
  uint32_t id_;
  std::shared_ptr<Socket> socket_;

  std::mutex input_queue_mutex_;
  std::queue<DataPtr> input_queue_;

  std::mutex output_queue_mutex_;
  std::queue<DataPtr> output_queue_;
  asio::steady_timer output_signal_;

private:
  Session(uint32_t id, std::shared_ptr<Socket> socket);

  asio::awaitable<void> write_buffers();
  asio::awaitable<void> read_buffers();

public:
  static std::shared_ptr<Session> create(uint32_t id, std::shared_ptr<Socket> socket);

  void close();
  void send(DataPtr buffer);
  void handle_input(std::function<void(DataPtr)> handler);

  bool connected();
};

class IoContextComponent
{
protected:
  std::shared_ptr<asio::io_context> io_context_;

  IoContextComponent(std::shared_ptr<asio::io_context> io_context) :
    io_context_(std::move(io_context)) {}

  virtual ~IoContextComponent() {}
};

class Server : protected IoContextComponent
{
public:
  using IoContextComponent::IoContextComponent;

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void update() = 0;
  virtual bool running() = 0;

  virtual void send(uint32_t session_id, DataPtr buffer) = 0;
  virtual void send_to_all(DataPtr buffer) = 0;
  virtual void close_session(uint32_t session_id) = 0;

  virtual void on_disconnect(uint32_t session_id) = 0;
  virtual void on_receive(uint32_t session_id, DataPtr buffer) = 0;
  virtual void on_connect(uint32_t session_id, std::shared_ptr<Session> session) = 0;
};

class Client : protected IoContextComponent
{
public:
  using IoContextComponent::IoContextComponent;

  virtual void connect() = 0;
  virtual void disconnect() = 0;
  virtual void update() = 0;
  virtual bool connected() = 0;
  virtual void send(DataPtr buffer) = 0;

  virtual void on_connect() = 0;
  virtual void on_disconnect() = 0;
  virtual void on_receive(DataPtr buffer) = 0;
};

NAMESPACE_NET_END
