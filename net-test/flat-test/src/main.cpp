
// #include <proto/net.h>
// #include <spdlog/spdlog.h>

// void test(const std::string& subject, const std::string& content)
// {
//   flatbuffers::FlatBufferBuilder builder;
//   auto message = net::CreateMessage(builder,
//     builder.CreateString(subject),
//     builder.CreateString(content)
//   );
//   builder.FinishSizePrefixed(message, net::MessageIdentifier());

//   uint8_t* buf = builder.GetBufferPointer();
//   const auto size = builder.GetSize();

//   std::string str(reinterpret_cast<const char*>(buf), size);

//   SPDLOG_INFO("Size: {}", size);
//   SPDLOG_INFO("Str: {}", str);


//   auto id = flatbuffers::GetBufferIdentifier(buf, true);
//   std::string id_str(id, flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

//   // flatbuffers::FlatBufferBuilder::sizePr

//   auto s = flatbuffers::GetSizePrefixedBufferLength(buf);
//   auto output = net::GetSizePrefixedMessage(buf);
//   SPDLOG_INFO("type: {}, size: {}, subject: {}, content: {}",
//     id_str, s, output->subject()->str(), output->content()->str());
// }

int main()
{
  // test("", "");
  // test("Hello", "World");
}
