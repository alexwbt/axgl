#pragma once

#include <spdlog/spdlog.h>
#include <net/flat/flat_server.h>
#include <proto/message.h>
#include <common/proto.h>

class NetServer : public net::FlatServer
{
public:
  NetServer(asio::ip::port_type port) :
    net::FlatServer(port)
  {
    auto mesg_handler = [](uint32_t session_id, net::TcpSession::DataPtr buffer)
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
    };
    buffer_handlers_.insert({ {proto::MessageIdentifier(), mesg_handler} });
  }

  void on_connect(uint32_t session_id, std::shared_ptr<net::TcpSession> session) override
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
};
