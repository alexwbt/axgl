#pragma once

#include <condition_variable>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <queue>

#include "net/asio.h"

namespace net
{

  class TcpSession final
  {
  public:
    typedef std::shared_ptr<std::vector<uint8_t>> DataPtr;

  private:
    uint32_t id_;
    asio::ip::tcp::socket socket_;

    std::mutex input_queue_mutex_;
    std::queue<DataPtr> input_queue_;

    std::mutex output_queue_mutex_;
    std::queue<DataPtr> output_queue_;
    asio::steady_timer output_signal_;

  public:
    static std::shared_ptr<TcpSession> create(uint32_t id, asio::ip::tcp::socket socket);

    void send(DataPtr buffer);
    void handle_input(std::function<void(DataPtr)> handler);
    void close();

    bool connected();

  public:
    TcpSession(uint32_t id, asio::ip::tcp::socket socket);

    asio::awaitable<void> write_buffers();
    asio::awaitable<void> read_buffers();
  };

  class TcpServer
  {
  private:
    const asio::ip::port_type port_;
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<uint32_t, std::shared_ptr<TcpSession>> sessions_;

    uint32_t next_session_id_ = 1;

  public:
    TcpServer(asio::ip::port_type port);
    virtual ~TcpServer() {}

    void start();
    void stop();
    void update();
    bool running();

    void send(uint32_t session_id, TcpSession::DataPtr buffer);
    void send_to_all(TcpSession::DataPtr buffer);
    void close_session(uint32_t session_id);

    virtual void on_connect(uint32_t session_id, std::shared_ptr<TcpSession> session) = 0;
    virtual void on_disconnect(uint32_t session_id) = 0;
    virtual void on_receive(uint32_t session_id, TcpSession::DataPtr buffer) = 0;

  private:
    uint32_t use_next_session_id();
    asio::awaitable<void> accept_connections();
  };

  class TcpClient
  {
  protected:
    asio::io_context io_context_;
    std::shared_ptr<TcpSession> session_;

  public:
    TcpClient(const std::string& host, asio::ip::port_type port);
    virtual ~TcpClient() {}

    void start();
    void stop();
    void update();
    bool running();
    void send(TcpSession::DataPtr buffer);

    virtual void on_connect() = 0;
    virtual void on_disconnect() = 0;
    virtual void on_receive(TcpSession::DataPtr buffer) = 0;
  };

}
