#pragma once

#include <demo_net_chatroom_fbs/message_fbs.h>

#include <net/common.hpp>

inline net::data_ptr_t build_message(const std::string& sender, const std::string& content)
{
  flatbuffers::FlatBufferBuilder builder;

  const auto sender_offset = builder.CreateString(sender);
  const auto content_offset = builder.CreateString(content);

  fbs::chatroom::MessageBuilder message_builder(builder);
  message_builder.add_sender(sender_offset);
  message_builder.add_content(content_offset);
  const auto message = message_builder.Finish();

  builder.Finish(message, fbs::chatroom::MessageIdentifier());

  auto buffer = builder.Release();
  return std::make_shared<net::data_t>(buffer.data(), buffer.data() + buffer.size());
}

inline const fbs::chatroom::Message* read_message(const net::data_ptr_t& buffer)
{
  constexpr int kOffset = net::LengthPrefixedTcpSocket::kLengthPrefixSize;
  const std::span data(buffer->data() + kOffset, buffer->size() - kOffset);

  const std::string identifier(
    flatbuffers::GetBufferIdentifier(data.data(), false), flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

  if (identifier != fbs::chatroom::MessageIdentifier()) return nullptr;
  if (auto verifier = flatbuffers::Verifier(data.data(), data.size()); !fbs::chatroom::VerifyMessageBuffer(verifier))
    return nullptr;

  const auto message = fbs::chatroom::GetMessage(data.data());
  return message;
}
