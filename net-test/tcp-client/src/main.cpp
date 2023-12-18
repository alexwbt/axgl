#include <iostream>
#include <spdlog/spdlog.h>

#include <net/tcp_server.h>
#include <proto/message.h>

using asio::ip::tcp;


flatbuffers::DetachedBuffer create_message(const std::string& content)
{
  flatbuffers::FlatBufferBuilder builder;

  auto content_offset = builder.CreateString(content);

  proto::MessageBuilder message_builder(builder);
  message_builder.add_content(content_offset);
  auto message = message_builder.Finish();

  builder.FinishSizePrefixed(message, proto::MessageIdentifier());

  // uint8_t* buf = builder.GetBufferPointer();
  // const auto size = builder.GetSize();
  // std::string str(reinterpret_cast<const char*>(buf), size);

  // SPDLOG_INFO("Create message. size: {}, str: {}", size, str);

  return builder.Release();
}


class Client : public net::TcpClient
{
public:
  using net::TcpClient::TcpClient;

  void on_connect() override
  {
    SPDLOG_INFO("Connected");

    send_message("what the fuck is wrong with you");
  }

  void on_disconnect() override
  {
    SPDLOG_INFO("Disconnected");
    io_context_.stop();
  }

  void on_receive(const std::string& identifier, std::vector<uint8_t> buffer) override
  {
    SPDLOG_INFO("Received {}", identifier);
  }

  void send_message(const std::string& message)
  {
    send(create_message(message));
  }

  void read_input()
  {
    // asio::co_spawn(io_context_, [this]() -> asio::awaitable<void>
    // {
    //   try
    //   {
    //     while (true)
    //     {
    //       std::string input;
    //       asio::posix::stream_descriptor input_stream(io_context_, 0);
    //       co_await asio::async_read(input_stream, asio::buffer(input), asio::use_awaitable);

    //       send_message(input);
    //     }
    //   }
    //   catch (const std::exception& e)
    //   {
    //     SPDLOG_ERROR(e.what());
    //   }
    // }, asio::detached);
  }
};


int main()
{
  try
  {
    Client client("127.0.0.1", 13000);

    client.start();

    // std::thread thread([&]()
    // {
    //   client.start();
    // });

    // while (true)
    // {
    //   std::string input;
    //   std::getline(std::cin, input);
    //   client.send_message(input);
    // }

    // client.stop();

    // if (thread.joinable())
    //   thread.join();
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
