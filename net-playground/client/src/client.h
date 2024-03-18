#pragma once

// #include <flatbuffer/flatbuffer.h>
#include <net/flat/flat_client.h>
#include <proto/message.h>
#include <common/proto.h>

#include <axgl/component.h>

#include "event.h"

class NetClient : public net::FlatTcpClient, public axgl::Component
{
  std::thread* client_thread_;

public:
  NetClient(const std::string& host, asio::ip::port_type port) :
    net::FlatTcpClient(host, port)
  {
    auto mesg_handler = [](net::DataPtr buffer)
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
    buffer_handlers_.insert({ {proto::MessageIdentifier(), mesg_handler} });
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

  void update(axgl::ComponentContext& context) override
  {
    const auto& events = context.get_events(EVENT_TYPE_SEND_NETWORK_MESSAGE);
    for (auto event : events)
      if (event->attributes.contains("message"))
        send_message(event->attributes.get<std::string>("message"));
  }

  void initialize(axgl::ComponentContext& context) override
  {
    client_thread_ = new std::thread([&]() { start(); });
  }

  void terminate(axgl::ComponentContext& context) override
  {
    stop();

    if (client_thread_->joinable())
      client_thread_->join();

    delete client_thread_;
  }
};
