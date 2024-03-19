#pragma once

#include <net/flat/tcp_adapter.h>
#include <axgl/component.h>
#include <proto/message.h>
#include <common/proto.h>

#include "event.h"

class NetClient : public net::flat::TcpClientAdapter, public axgl::Component
{
private:
  std::shared_ptr<std::thread> client_thread_;

public:
  NetClient(std::shared_ptr<asio::io_context> io_context) :
    net::flat::TcpClientAdapter(io_context)
  {
    add_handler(proto::MessageIdentifier(), [](uint32_t, net::DataPtr buffer)
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
    });
  }

  void on_connect() override
  {
    SPDLOG_INFO("Connected");
  }

  void on_disconnect() override
  {
    SPDLOG_INFO("Disconnected");
  }

  void connection_failed(const asio::error_code& error_code) override
  {
    SPDLOG_ERROR("Connection failed ({})", error_code.message());
  }

  void send_message(const std::string& message)
  {
    send(create_message(message));
  }

  void update(axgl::ComponentContext& context) override
  {
    net::flat::TcpClientAdapter::update();

    const auto& send_message_events = context.get_events(EVENT_TYPE_SEND_NETWORK_MESSAGE);
    for (auto event : send_message_events)
      if (event->attributes.contains("message"))
        send_message(event->attributes.get<std::string>("message"));

    const auto& connect_events = context.get_events(EVENT_TYPE_CONNECT_SERVER);
    for (auto event : connect_events)
    {
      if (event->attributes.contains("host") && event->attributes.contains("port"))
      {
        connect(
          event->attributes.get<std::string>("host"),
          event->attributes.get<uint32_t>("port")
        );
        break;
      }
    }

    const auto& disconnect_events = context.get_events(EVENT_TYPE_DISCONNECT_SERVER);
    if (!disconnect_events.empty())
      disconnect();
  }

  void connect(const std::string& host, const asio::ip::port_type& port) override
  {
    disconnect();

    SPDLOG_INFO("Connecting to {}:{}", host, port);
    net::flat::TcpClientAdapter::connect(host, port);

    client_thread_ = std::make_shared<std::thread>([this]()
    {
      try
      {
        io_context_->restart();
        io_context_->run();
      }
      catch (const std::exception& e)
      {
        SPDLOG_CRITICAL(e.what());
      }
    });
  }

  void disconnect() override
  {
    net::flat::TcpClientAdapter::disconnect();

    if (io_context_ && !io_context_->stopped())
      io_context_->stop();

    if (client_thread_ && client_thread_->joinable())
      client_thread_->join();
  }
};
