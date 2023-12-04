#include <chrono>
#include <format>

#include <spdlog/spdlog.h>

#include <net/tcp_server.h>


class Server : public net::TcpServer
{
public:
  std::string get_daytime_string()
  {
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%d-%m-%Y %H:%M:%OS}", now);
  }

  void on_new_connection(asio::ip::tcp::socket socket)
  {
    SPDLOG_INFO("new connection from {}", socket.remote_endpoint().address().to_string());

    asio::error_code ignored_error;
    asio::write(socket, asio::buffer(get_daytime_string()), ignored_error);
  }
};

int main()
{
  try
  {
    asio::io_context io_context;

    constexpr uint16_t PORT = 13000;

    Server server;
    server.start(io_context, PORT);
    SPDLOG_INFO("starting server on {}", PORT);

    asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&io_context](auto, auto) { io_context.stop(); });

    io_context.run();
    SPDLOG_INFO("server stopped", PORT);
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
