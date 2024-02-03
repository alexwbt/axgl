#pragma once

#include <spdlog/spdlog.h>
#include <net/tcp_session.h>
#include <proto/message.h>
#include "common/proto.h"

class Server : public net::TcpServer
{
  using net::TcpServer::TcpServer;

public:
  void on_connect(uint32_t session_id, std::shared_ptr<net::TcpSession> session) override
  {
    SPDLOG_INFO("new connection (id: {})", session_id);
  }

  void on_disconnect(uint32_t session_id) override
  {
    SPDLOG_INFO("client disconnected (id: {})", session_id);
  }

  void on_receive(uint32_t session_id, net::TcpSession::DataPtr buffer) override
  {
    std::string identifier(
      flatbuffers::GetBufferIdentifier(buffer->data(), true),
      flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

    SPDLOG_INFO("(client: {}) Received {}", session_id, identifier);
    if (identifier != "MESG")
      return;

    auto verifier = flatbuffers::Verifier(buffer->data(), buffer->size());
    auto is_valid_message = proto::VerifySizePrefixedMessageBuffer(verifier);
    if (!is_valid_message)
    {
      SPDLOG_WARN("(client: {}) Received invalid message", session_id);
      return;
    }

    auto message = proto::GetSizePrefixedMessage(buffer->data());
    SPDLOG_INFO("(client: {}) Message Content: {}", session_id, message->content()->str());
  }

  void send_message(const std::string& message)
  {
    send_to_all(create_message(message));
  }
};
