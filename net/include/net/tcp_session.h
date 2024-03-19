#pragma once

#include "net/session.h"

#include <unordered_map>

NAMESPACE_NET

class TcpServer : public Server
{
protected:
  const asio::ip::port_type port_;
  asio::ip::tcp::acceptor acceptor_;
  std::unordered_map<uint32_t, std::shared_ptr<Session>> sessions_;

  uint32_t next_session_id_ = 1;

public:
  TcpServer(std::shared_ptr<asio::io_context> io_context,
    asio::ip::port_type port);

  void start() override;
  void stop() override;
  void update() override;
  bool running() override;

  void send(uint32_t session_id, DataPtr buffer) override;
  void send_to_all(DataPtr buffer) override;
  void close_session(uint32_t session_id) override;

  virtual std::shared_ptr<Socket> new_socket(asio::ip::tcp::socket socket) = 0;

private:
  uint32_t use_next_session_id();
  asio::awaitable<void> accept_connections();
};

class TcpClient : public Client
{
protected:
  std::string host_;
  asio::ip::port_type port_;
  std::shared_ptr<Session> session_;

public:
  TcpClient(std::shared_ptr<asio::io_context> io_context,
    const std::string& host, asio::ip::port_type port);

  void connect() override;
  void disconnect() override;
  void update() override;
  bool connected() override;
  void send(DataPtr buffer) override;

  virtual std::shared_ptr<Socket> new_socket(asio::ip::tcp::socket socket) = 0;
  virtual void connection_failed(const asio::error_code& error_code) = 0;
};

NAMESPACE_NET_END
