#pragma once

#include <demo_net_chatroom_fbs/message_fbs.h>

#include <net/net.hpp>

inline net::DataPtr build_message(const std::string& content)
{
  flatbuffers::FlatBufferBuilder builder;

  const auto content_offset = builder.CreateString(content);

  fbs::chatroom::MessageBuilder message_builder(builder);
  message_builder.add_content(content_offset);
  const auto message = message_builder.Finish();

  builder.FinishSizePrefixed(message, fbs::chatroom::MessageIdentifier());

  auto buffer = builder.Release();
  return std::make_shared<net::DataContainer>(buffer.data(), buffer.data() + buffer.size());
}
