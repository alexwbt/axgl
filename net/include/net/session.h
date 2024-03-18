#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

#include "net/asio.h"

namespace net
{
  typedef std::shared_ptr<std::vector<uint8_t>> DataPtr;

  class Socket
  {
  public:
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

}
