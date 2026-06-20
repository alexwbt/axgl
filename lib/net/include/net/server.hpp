#pragma once

#include <ranges>
#include <unordered_map>

#include <net/common.hpp>
#include <net/session.hpp>

namespace net
{

class Server
{
  uint32_t next_session_id_ = 1;

protected:
  std::shared_ptr<asio::io_context> io_context_;

  const asio::ip::port_type port_;
  std::unordered_map<uint32_t, std::shared_ptr<Session>> sessions_;

public:
  virtual ~Server() = default;

  Server(std::shared_ptr<asio::io_context> io_context, const asio::ip::port_type& port) :
    io_context_(std::move(io_context)), port_(port)
  {
  }

  virtual void update()
  {
    for (auto it = sessions_.begin(); it != sessions_.end();)
    {
      it->second->handle_input([this, &it](data_ptr_t buffer) { on_receive(it->first, std::move(buffer)); });

      if (!it->second->connected())
      {
        on_disconnect(it->first);
        it = sessions_.erase(it);
        continue;
      }
      ++it;
    }
  }

  virtual void send(const uint32_t session_id, data_ptr_t buffer)
  {
    if (sessions_.contains(session_id)) sessions_.at(session_id)->send(std::move(buffer));
  }

  virtual void send_to_all(data_ptr_t buffer)
  {
    for (const auto& val : sessions_ | std::views::values)
      val->send(buffer);
  }

  virtual void close_session(const uint32_t session_id)
  {
    if (sessions_.contains(session_id)) sessions_.at(session_id)->close();
  }

  virtual void on_disconnect(uint32_t session_id) { }
  virtual void on_receive(uint32_t session_id, data_ptr_t buffer) { }
  virtual void on_connect(uint32_t session_id, std::shared_ptr<Session> session) { }

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual bool running() = 0;

protected:
  uint32_t use_next_session_id() { return next_session_id_++; }
};

} // namespace net
