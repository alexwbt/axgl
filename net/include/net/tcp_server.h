#pragma once

#include <asio.hpp>

namespace net {

  class TcpServer
  {
  public:
    TcpServer() {}
    virtual ~TcpServer() {}

    void start(asio::io_context& io_context, asio::ip::port_type port);

    virtual void on_new_connection(asio::ip::tcp::socket socket) {}
  };

}
