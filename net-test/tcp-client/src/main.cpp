// #include <iostream>

// #include <spdlog/spdlog.h>

// #include <net/tcp.h>
// #include <proto/message.h>


// std::shared_ptr<std::vector<uint8_t>> create_message(const std::string& content)
// {
//   flatbuffers::FlatBufferBuilder builder;

//   auto content_offset = builder.CreateString(content);

//   proto::MessageBuilder message_builder(builder);
//   message_builder.add_content(content_offset);
//   auto message = message_builder.Finish();

//   builder.FinishSizePrefixed(message, proto::MessageIdentifier());

//   auto buffer = builder.Release();
//   return std::make_shared<std::vector<uint8_t>>(
//     buffer.data(), buffer.data() + buffer.size());
// }


// class Client : public net::TcpClient
// {
// public:
//   using net::TcpClient::TcpClient;

//   void on_connect() override
//   {
//     SPDLOG_INFO("Connected");
//   }

//   void on_disconnect() override
//   {
//     SPDLOG_INFO("Disconnected");
//   }

//   void on_receive(net::DataPtr buffer) override
//   {
//     std::string identifier(
//       flatbuffers::GetBufferIdentifier(buffer->data(), true),
//       flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

//     SPDLOG_INFO("Received {}", identifier);
//     if (identifier != "MESG")
//       return;

//     auto verifier = flatbuffers::Verifier(buffer->data(), buffer->size());
//     auto is_valid_message = proto::VerifySizePrefixedMessageBuffer(verifier);
//     if (!is_valid_message)
//     {
//       SPDLOG_WARN("Received invalid message");
//       return;
//     }

//     auto message = proto::GetSizePrefixedMessage(buffer->data());
//     SPDLOG_INFO("Message Content: {}", message->content()->str());
//   }

//   void send_message(const std::string& message)
//   {
//     send(create_message(message));
//   }
// };


// int main()
// {
//   try
//   {
//     Client client("127.0.0.1", 13000);

//     std::thread thread([&]()
//     {
//       client.start();
//     });

//     std::thread cin_thread([&]()
//     {
//       while (true)
//       {
//         std::string input;
//         std::getline(std::cin, input);
//         if (input.empty())
//           continue;

//         if (input == "/exit")
//         {
//           client.stop();
//           break;
//         }

//         client.send_message(input);
//       }
//     });

//     while (client.running())
//       client.update();

//     if (thread.joinable())
//       thread.join();

//     if (cin_thread.joinable())
//       cin_thread.join();
//   }
//   catch (const std::exception& e)
//   {
//     SPDLOG_CRITICAL(e.what());
//   }
// }

int main() {}
