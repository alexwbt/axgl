#include <args.hxx>
#include <spdlog/spdlog.h>

#include <net/tcp/server.hpp>
#include <net/tcp/socket.hpp>

#include <demo_net_chatroom_fbs/message_fbs.h>

#include "fbs/message.hpp"

class Server final : public net::TcpServer
{
public:
  using TcpServer::TcpServer;

  std::shared_ptr<net::Socket> new_socket(asio::ip::tcp::socket socket) override
  {
    return std::make_shared<net::LengthPrefixedTcpSocket>(std::move(socket));
  }

  void on_connect(const uint32_t session_id, const std::shared_ptr<net::Session>&) override
  {
    SPDLOG_INFO("new connection (id: {})", session_id);
  }

  void on_disconnect(const uint32_t session_id) override { SPDLOG_INFO("client disconnected (id: {})", session_id); }

  void on_receive(uint32_t session_id, const net::data_ptr_t& buffer) override
  {
    const auto* message = read_message(buffer);

    if (!message)
    {
      SPDLOG_WARN("(client: {}) Received invalid message", session_id);
      return;
    }

    const auto& sender = message->sender()->str();
    const auto& content = message->content()->str();

    SPDLOG_INFO("(client: {}) Message: <{}> {}", session_id, sender, content);
    if (!sender.empty()) send_to_all(build_message(sender, content));
  }
};

int main(int argc, char** argv)
{
  args::ArgumentParser parser("Net demo chatroom server.");
  args::ValueFlag<std::uint16_t> port_arg(parser, "port", "Optional port.", {'p', "port"}, 10000);
  try
  {
    parser.ParseCLI(argc, argv);

    const auto& port = args::get(port_arg);

    const auto io_context = std::make_shared<asio::io_context>();

    Server server(io_context, port);
    server.start();

    SPDLOG_INFO("Server is running on port: {}", port);

    std::thread thread([&]
    {
      try
      {
        io_context->run();
      }
      catch (const std::exception& e)
      {
        SPDLOG_ERROR(e.what());
      }
    });

    while (server.running())
      server.update();

    if (thread.joinable()) thread.join();
  }
  catch (const args::Completion& e)
  {
    std::cout << e.what();
  }
  catch (const args::Help&)
  {
    std::cout << parser;
  }
  catch (const args::Error& e)
  {
    SPDLOG_ERROR(e.what());
    return 1;
  }
  catch (const std::exception& e)
  {
    SPDLOG_ERROR(e.what());
    return 1;
  }
}
