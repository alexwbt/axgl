#pragma once

#include "net/asio_application.h"

namespace net {

  class TcpServer : private AsioApplication
  {
  private:
    asio::ip::tcp::acceptor acceptor_;

  public:
    TcpServer(asio::ip::port_type port);

    void start();

    virtual void on_new_connection(asio::ip::tcp::socket socket) {}
  };

}
