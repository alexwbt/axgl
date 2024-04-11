// #include <chrono>
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


// class Server : public net::TcpServer
// {
//   using net::TcpServer::TcpServer;

// public:
//   void on_connect(uint32_t session_id, std::shared_ptr<net::Session> session) override
//   {
//     SPDLOG_INFO("new connection (id: {})", session_id);
//   }

//   void on_disconnect(uint32_t session_id) override
//   {
//     SPDLOG_INFO("client disconnected (id: {})", session_id);
//   }

//   void on_receive(uint32_t session_id, net::DataPtr buffer) override
//   {
//     std::string identifier(
//       flatbuffers::GetBufferIdentifier(buffer->data(), true),
//       flatbuffers::FlatBufferBuilder::kFileIdentifierLength);

//     SPDLOG_INFO("(client: {}) Received {}", session_id, identifier);
//     if (identifier != "MESG")
//       return;

//     auto verifier = flatbuffers::Verifier(buffer->data(), buffer->size());
//     auto is_valid_message = proto::VerifySizePrefixedMessageBuffer(verifier);
//     if (!is_valid_message)
//     {
//       SPDLOG_WARN("(client: {}) Received invalid message", session_id);
//       return;
//     }

//     auto message = proto::GetSizePrefixedMessage(buffer->data());
//     SPDLOG_INFO("(client: {}) Message Content: {}", session_id, message->content()->str());
//   }

//   void send_message(const std::string& message)
//   {
//     send_to_all(create_message(message));
//   }
// };

// int main()
// {
//   try
//   {
//     Server server(13000);

//     std::thread thread([&]()
//     {
//       server.start();
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
//           server.stop();
//           break;
//         }

//         server.send_message(input);
//       }
//     });

//     while (server.running())
//       server.update();

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
