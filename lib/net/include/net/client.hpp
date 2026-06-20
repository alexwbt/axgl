#pragma once

#include <net/common.hpp>
#include <net/session.hpp>

namespace net
{

class Client
{
protected:
  std::shared_ptr<Session> session_;
  std::shared_ptr<asio::io_context> io_context_;

public:
  virtual ~Client() = default;

  explicit Client(std::shared_ptr<asio::io_context> io_context) : io_context_(std::move(io_context)) { }

  virtual void disconnect()
  {
    if (session_) session_->close();
  }

  virtual void update()
  {
    if (!session_) return;

    session_->handle_input([this](const data_ptr_t& buffer) { on_receive(buffer); });

    if (!session_->connected())
    {
      on_disconnect();
      session_ = nullptr;
    }
  }

  virtual bool connected() { return session_ && session_->connected(); }

  virtual void send(data_ptr_t buffer)
  {
    if (session_) session_->send(std::move(buffer));
  }

  virtual void on_connect() { }
  virtual void on_disconnect() { }
  virtual void on_receive(const data_ptr_t&) { }
  virtual void connection_failed(const asio::error_code&) { }

  virtual void connect(const std::string& host, const asio::ip::port_type& port) = 0;
};

} // namespace net
