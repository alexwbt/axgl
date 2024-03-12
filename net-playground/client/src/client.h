#pragma once

// #include <flatbuffer/flatbuffer.h>
#include <net/flat/flat_client.h>
#include <proto/message.h>
#include <common/proto.h>

class NetClient : public net::FlatClient
{
public:
  NetClient(const std::string& host, asio::ip::port_type port) :
    net::FlatClient(host, port)
  {
    auto mesg_handler = [](net::TcpSession::DataPtr buffer)
    {
      auto verifier = flatbuffers::Verifier(buffer->data(), buffer->size());
      auto is_valid_message = proto::VerifySizePrefixedMessageBuffer(verifier);
      if (!is_valid_message)
      {
        SPDLOG_WARN("Received invalid message");
        return;
      }

      auto message = proto::GetSizePrefixedMessage(buffer->data());
      SPDLOG_INFO("Message Content: {}", message->content()->str());
    };
    buffer_handlers_.insert({ {"MESG", mesg_handler} });
  }

  void on_connect() override
  {
    SPDLOG_INFO("Connected");
  }

  void on_disconnect() override
  {
    SPDLOG_INFO("Disconnected");
  }

  void send_message(const std::string& message)
  {
    send(create_message(message));
  }
};
