#include "common/proto.h"

// #include <net/tcp_session.h>
#include <proto/message.h>


std::shared_ptr<std::vector<uint8_t>> create_message(const std::string& content)
{
  flatbuffers::FlatBufferBuilder builder;

  auto content_offset = builder.CreateString(content);

  proto::MessageBuilder message_builder(builder);
  message_builder.add_content(content_offset);
  auto message = message_builder.Finish();

  builder.FinishSizePrefixed(message, proto::MessageIdentifier());

  auto buffer = builder.Release();
  return std::make_shared<std::vector<uint8_t>>(
    buffer.data(), buffer.data() + buffer.size());
}
