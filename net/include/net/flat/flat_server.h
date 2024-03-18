#pragma once

#include <net/tcp_session.h>

#include <string>
#include <functional>
#include <unordered_map>

#include <flatbuffers/flatbuffers.h>

namespace net
{

  class FlatTcpServer : public TcpServer
  {
  public:
    typedef std::function<void(uint32_t, TcpSession::DataPtr)> BufferHandler;

  protected:
    std::unordered_map<std::string, BufferHandler> buffer_handlers_;

  public:
    using TcpServer::TcpServer;
    void on_connect(uint32_t session_id, std::shared_ptr<TcpSession> session) override {}
    void on_disconnect(uint32_t session_id) override {}

    void on_receive(uint32_t session_id, TcpSession::DataPtr buffer) override
    {
      std::string identifier(
        flatbuffers::GetBufferIdentifier(buffer->data(), true),
        flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

      if (buffer_handlers_.contains(identifier))
        buffer_handlers_.at(identifier)(session_id, buffer);
    }
  };

}
