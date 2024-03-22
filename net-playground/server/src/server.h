#pragma once

#include <spdlog/spdlog.h>

#include <net/flat/tcp_adapter.hpp>

#include <proto/message.h>
#include <common/proto.h>

class NetServer : public net::flat::TcpServerAdapter
{
public:
  NetServer(std::shared_ptr<asio::io_context> io_context, asio::ip::port_type port) :
    net::flat::TcpServerAdapter(io_context, port)
  {
    add_handler(proto::MessageIdentifier(), [](uint32_t session_id, net::DataPtr buffer)
    {
      auto verifier = flatbuffers::Verifier(buffer->data(), buffer->size());
      auto is_valid_message = proto::VerifySizePrefixedMessageBuffer(verifier);
      if (!is_valid_message)
      {
        SPDLOG_WARN("Received invalid message");
        return;
      }

      auto message = proto::GetSizePrefixedMessage(buffer->data());
      SPDLOG_INFO("(client: {}) Message Content: {}", session_id, message->content()->str());
    });
  }

  void on_connect(uint32_t session_id, std::shared_ptr<net::Session> session) override
  {
    SPDLOG_INFO("new connection (id: {})", session_id);
  }

  void on_disconnect(uint32_t session_id) override
  {
    SPDLOG_INFO("client disconnected (id: {})", session_id);
  }

  void send_message(const std::string& message)
  {
    send_to_all(create_message(message));
  }

  void start() override
  {
    SPDLOG_INFO("starting server on {}", port_);
    net::flat::TcpServerAdapter::start();
  }
};
