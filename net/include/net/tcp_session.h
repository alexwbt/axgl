#pragma once

#include <unordered_map>
#include <flatbuffers/flatbuffers.h>

#include "net/session.h"

namespace net
{

  class TcpFlatBufferSocket final : public Socket
  {
  private:
    asio::ip::tcp::socket socket_;

  public:
    TcpFlatBufferSocket(asio::ip::tcp::socket socket) :
      socket_(std::move(socket)) {}

    asio::awaitable<void> write_buffer(DataPtr buffer)
    {
      co_await asio::async_write(socket_,
        asio::buffer(buffer->data(), buffer->size()),
        asio::use_awaitable);
    }

    asio::awaitable<void> read_buffer(std::vector<uint8_t>& buffer)
    {
      // read size
      co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, 4), asio::use_awaitable);

      auto size = flatbuffers::GetSizePrefixedBufferLength(buffer.data());

      // read all of size
      co_await asio::async_read(socket_, asio::dynamic_buffer(buffer, size), asio::use_awaitable);
    }

    void close() { socket_.close(); }
    bool connected() { return socket_.is_open(); }

    asio::any_io_executor get_executor() { return socket_.get_executor(); }
  };

  class TcpServer
  {
  private:
    const asio::ip::port_type port_;
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<uint32_t, std::shared_ptr<Session>> sessions_;

    uint32_t next_session_id_ = 1;

  public:
    TcpServer(asio::ip::port_type port);
    virtual ~TcpServer() {}

    void start();
    void stop();
    void update();
    bool running();

    void send(uint32_t session_id, DataPtr buffer);
    void send_to_all(DataPtr buffer);
    void close_session(uint32_t session_id);

    virtual void on_connect(uint32_t session_id, std::shared_ptr<Session> session) = 0;
    virtual void on_disconnect(uint32_t session_id) = 0;
    virtual void on_receive(uint32_t session_id, DataPtr buffer) = 0;

  private:
    uint32_t use_next_session_id();
    asio::awaitable<void> accept_connections();
  };

  class TcpClient
  {
  protected:
    asio::io_context io_context_;
    std::shared_ptr<Session> session_;

  public:
    TcpClient(const std::string& host, asio::ip::port_type port);
    virtual ~TcpClient() {}

    void start();
    void stop();
    void update();
    bool running();
    void send(DataPtr buffer);

    virtual void on_connect() = 0;
    virtual void on_disconnect() = 0;
    virtual void on_receive(DataPtr buffer) = 0;
  };

}
