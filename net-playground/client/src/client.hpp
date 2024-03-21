#pragma once

#include <net/flat/tcp_adapter.h>
#include <axgl/component.h>
#include <proto/message.h>
#include <common/proto.h>

#include "event.hpp"

class NetClient : public net::flat::TcpClientAdapter, public axgl::Component
{
private:
  std::shared_ptr<std::thread> client_thread_;
  
  std::vector<std::string> messages_;

public:
  NetClient(std::shared_ptr<asio::io_context> io_context) :
    net::flat::TcpClientAdapter(io_context)
  {
    add_handler(proto::MessageIdentifier(), [this](uint32_t, net::DataPtr buffer)
    {
      auto verifier = flatbuffers::Verifier(buffer->data(), buffer->size());
      auto is_valid_message = proto::VerifySizePrefixedMessageBuffer(verifier);
      if (!is_valid_message)
      {
        messages_.push_back("Received invalid message");
        return;
      }

      auto message = proto::GetSizePrefixedMessage(buffer->data());
      messages_.push_back("Message Content: " + message->content()->str());
    });
  }

  void on_connect() override
  {
    messages_.push_back("Connected");
  }

  void on_disconnect() override
  {
    messages_.push_back("Disconnected");
  }

  void connection_failed(const asio::error_code& error_code) override
  {
    messages_.push_back("Connection failed: " + error_code.message());
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

    for (const auto& message : messages_)
      event::console_log(context, message);
    messages_.clear();
  }

  void connect(const std::string& host, const asio::ip::port_type& port) override
  {
    disconnect();

    messages_.push_back("Connecting to " + host + ":" + std::to_string(port));
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
