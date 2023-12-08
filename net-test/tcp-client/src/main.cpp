#include <spdlog/spdlog.h>
#include <sstream>

#include <net/asio.h>
#include <proto/message.h>

using asio::ip::tcp;


flatbuffers::DetachedBuffer create_message(const std::string& subject, const std::string& content)
{
  flatbuffers::FlatBufferBuilder builder;

  auto subject_offset = builder.CreateString(subject);
  auto content_offset = builder.CreateString(content);

  proto::MessageBuilder message_builder(builder);
  message_builder.add_subject(subject_offset);
  message_builder.add_content(content_offset);
  auto message = message_builder.Finish();

  builder.FinishSizePrefixed(message, proto::MessageIdentifier());

  uint8_t* buf = builder.GetBufferPointer();
  const auto size = builder.GetSize();
  std::string str(reinterpret_cast<const char*>(buf), size);

  SPDLOG_INFO("Create message. size: {}, str: {}", size, str);

  return builder.Release();
}


int main()
{
  try
  {
    auto message1 = create_message("Hello", "World");
    auto message2 = create_message("what", "I decided that I need to write a wrapper over the socket, which inside itself can store the remainder of the call async_read_until, and then successfully use this buffer in subsequent calls of my function.");

    asio::io_context io_context;
    tcp::resolver resolver(io_context);

    tcp::socket socket(io_context);
    asio::connect(socket, resolver.resolve("localhost", "13000"));

    SPDLOG_INFO("Connected");

    asio::error_code ignored_error;
    asio::write(socket, asio::buffer(message1.data(), message1.size()), ignored_error);
    asio::write(socket, asio::buffer(message2.data(), message2.size()), ignored_error);

    // std::stringstream stringstream;
    // while (true)
    // {
    //   std::vector<char> buf(128);
    //   asio::error_code error;

    //   size_t len = socket.read_some(asio::buffer(buf), error);
    //   if (error == asio::error::eof)
    //     break; // Connection closed cleanly by peer.
    //   else if (error)
    //     throw asio::system_error(error); // Some other error.

    //   stringstream.write(buf.data(), len);
    // }

    // SPDLOG_INFO("Received daytime: {}", stringstream.str());
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
