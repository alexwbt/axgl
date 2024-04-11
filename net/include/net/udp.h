#pragma once

#include "net/net.h"

NAMESPACE_NET

class UdpServer : public Server
{
protected:
  asio::ip::udp::socket socket_;

public:
  UdpServer(std::shared_ptr<asio::io_context> io_context, const asio::ip::port_type& port);

  void start() override;
  // void stop() override;
  // bool running() override;

  // virtual std::shared_ptr<Socket> new_socket(asio::ip::tcp::socket socket) = 0;

// private:
//   asio::awaitable<void> accept_connections();
};

class UdpClient : public Client
{
public:
  // using Client::Client;

  // void connect(const std::string& host, const asio::ip::port_type& port) override;

  // virtual std::shared_ptr<Socket> new_socket(asio::ip::tcp::socket socket) = 0;
};

NAMESPACE_NET_END
