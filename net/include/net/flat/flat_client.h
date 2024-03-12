#pragma once

#include <net/tcp_session.h>

#include <string>
#include <functional>
#include <unordered_map>

#include <flatbuffers/flatbuffers.h>

namespace net
{

  class FlatClient : public TcpClient
  {
  public:
    typedef std::function<void(TcpSession::DataPtr)> BufferHandler;

  protected:
    std::unordered_map<std::string, BufferHandler> buffer_handlers_;

  public:
    using TcpClient::TcpClient;
    void on_connect() override {}
    void on_disconnect() override {}

    void on_receive(TcpSession::DataPtr buffer) override
    {
      std::string identifier(
        flatbuffers::GetBufferIdentifier(buffer->data(), true),
        flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

      if (buffer_handlers_.contains(identifier))
        buffer_handlers_.at(identifier)(buffer);
    }
  };

}
