#pragma once

#include <spdlog/spdlog.h>
#include <net/flat/tcp_adapter.h>
#include <proto/message.h>
#include <common/proto.h>

class NetServer : public net::flat::TcpServerAdapter
{
public:
  NetServer(asio::ip::port_type port) :
    net::flat::TcpServerAdapter(port)
  {
    auto mesg_handler = [](uint32_t session_id, net::DataPtr buffer)
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

  void start()
  {
    try
    {
      SPDLOG_INFO("starting server on {}", port());

      net::flat::TcpServerAdapter::start();

      SPDLOG_INFO("server stopped");
    }
    catch (const std::exception& e)
    {
      SPDLOG_CRITICAL(e.what());
    }
  }
};
