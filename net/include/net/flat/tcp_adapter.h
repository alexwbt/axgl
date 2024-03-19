#pragma once

#include "net/tcp_session.h"

#include <flatbuffers/flatbuffers.h>

NAMESPACE_NET_FLAT

class TcpSocket final : public Socket
{
private:
  asio::ip::tcp::socket socket_;

public:
  TcpSocket(asio::ip::tcp::socket socket) :
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

class FlatBufferHandler
{
public:
  typedef std::function<void(uint32_t, DataPtr)> BufferHandler;

private:
  std::unordered_map<std::string, BufferHandler> buffer_handlers_;

protected:
  void add_handler(const std::string& identifier, BufferHandler handler)
  {
    buffer_handlers_.insert({ {identifier, handler} });
  }

  void on_receive(uint32_t session_id, DataPtr buffer)
  {
    std::string identifier(
      flatbuffers::GetBufferIdentifier(buffer->data(), true),
      flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

    if (buffer_handlers_.contains(identifier))
      buffer_handlers_.at(identifier)(session_id, buffer);
  }
};

class TcpServerAdapter : public TcpServer, protected FlatBufferHandler
{
public:
  using TcpServer::TcpServer;
  void on_connect(uint32_t session_id, std::shared_ptr<Session> session) override {}
  void on_disconnect(uint32_t session_id) override {}

  void on_receive(uint32_t session_id, DataPtr buffer) override
  {
    FlatBufferHandler::on_receive(session_id, std::move(buffer));
  }

  std::shared_ptr<Socket> new_socket(asio::ip::tcp::socket socket) override
  {
    return std::make_shared<TcpSocket>(std::move(socket));
  }
};

class TcpClientAdapter : public TcpClient, protected FlatBufferHandler
{
public:
  typedef std::function<void(DataPtr)> BufferHandler;

protected:
  std::unordered_map<std::string, BufferHandler> buffer_handlers_;

public:
  using TcpClient::TcpClient;
  void on_connect() override {}
  void on_disconnect() override {}
  void connection_failed(const asio::error_code& error_code) override {}

  void on_receive(DataPtr buffer) override
  {
    FlatBufferHandler::on_receive(0, std::move(buffer));
  }

  std::shared_ptr<Socket> new_socket(asio::ip::tcp::socket socket) override
  {
    return std::make_shared<TcpSocket>(std::move(socket));
  }
};

NAMESPACE_NET_FLAT_END
