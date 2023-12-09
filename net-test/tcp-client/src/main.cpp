#include <spdlog/spdlog.h>
#include <iostream>

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
  Client() : net::TcpClient("127.0.0.1", 13000)
  {
    SPDLOG_INFO("Connected");
    start();
  }

  void disconnect(uint32_t session_id) override
  {
    SPDLOG_INFO("Disconnected");
  }

  void on_receive(uint32_t session_id, const std::string& identifier, std::vector<uint8_t> buffer) override
  {
    SPDLOG_INFO("Received {}", identifier);
  }

  void send_message(const std::string& message)
  {
    send(create_message(message));
  }
};


int main()
{
  try
  {
    Client client;

    while (true)
    {
        std::string input;
        std::getline(std::cin, input);

        client.send_message(input);
    }
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
