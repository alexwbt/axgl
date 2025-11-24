
#include "common/message.hpp"

class Server final : public net::TcpServer
{
  bool stop_ = false;

public:
  using TcpServer::TcpServer;

  std::shared_ptr<net::Socket> new_socket(asio::ip::tcp::socket socket) override
  {
    return std::make_shared<net::LengthPrefixedTcpSocket>(std::move(socket));
  }

  void on_connect(const uint32_t session_id, std::shared_ptr<net::Session> session) override
  {
    SPDLOG_DEBUG("client connected: {}", session_id);
  }

  void on_disconnect(const uint32_t session_id) override { SPDLOG_DEBUG("client disconnected: {}", session_id); }

  void on_receive(uint32_t session_id, const net::DataPtr buffer) override
  {
    print_message(*buffer);
    send_to_all(build_message("shut up."));
    stop_ = true;
  }

  void update() override
  {
    if (stop_)
    {
      stop();
      return;
    }
    TcpServer::update();
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

    Server server(io_context, 10000);
    server.start();

    std::thread thread(
      [&]
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

    while (server.running())
      server.update();

    if (thread.joinable())
      thread.join();
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
