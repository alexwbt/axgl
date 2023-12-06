#pragma once

#include <unordered_map>

#include "net/asio.h"

namespace net
{

  class TcpConnection
  {

  };

  class TcpServer
  {
  private:
    const asio::ip::port_type port_;
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<uint32_t, std::shared_ptr<asio::ip::tcp::socket>> sockets_;

    uint32_t next_socket_id_ = 0;

  public:
    TcpServer(asio::ip::port_type port);
    virtual ~TcpServer() {}

    void start();
    void stop();
    void remove_socket(uint32_t id);

    virtual void on_new_connection(uint32_t id, std::shared_ptr<asio::ip::tcp::socket> socket) = 0;

  private:
    uint32_t use_next_socket_id();
    asio::awaitable<void> accept_connections();
    asio::awaitable<void> read_from_socket(uint32_t socket_id);
  };

}
