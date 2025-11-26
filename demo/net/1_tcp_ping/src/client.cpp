
#include "common/message.hpp"

class Client final : public net::TcpClient
{
public:
  bool stop = false;
  using TcpClient::TcpClient;

  std::shared_ptr<net::Socket> new_socket(asio::ip::tcp::socket socket) override
  {
    return std::make_shared<net::LengthPrefixedTcpSocket>(std::move(socket));
  }

  void connection_failed(const asio::error_code& error_code) override
  {
    SPDLOG_ERROR("connection failed: {}", error_code.message());
  }

  void on_disconnect() override { SPDLOG_DEBUG("disconnected"); }

  void on_connect() override
  {
    SPDLOG_DEBUG("connected");
    send(build_message("hello world"));
  }

  void on_receive(const net::DataPtr buffer) override
  {
    print_message(*buffer);
    stop = true;
  }
};

int main()
{
#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_DEBUG
  spdlog::set_level(spdlog::level::debug);
#endif

  try
  {
    const auto io_context = std::make_shared<asio::io_context>();
    Client client(io_context);

    std::thread thread([&]
    {
      try
      {
        io_context->run();
      }
      catch (const std::exception& e)
      {
        SPDLOG_CRITICAL(e.what());
      }
    });

    client.connect("127.0.0.1", 10000);

    while (!client.stop)
      client.update();

    client.disconnect();

    if (!io_context->stopped())
      io_context->stop();

    if (thread.joinable())
      thread.join();
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
