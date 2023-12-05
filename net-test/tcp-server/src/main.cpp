#include <chrono>
#include <format>

#include <spdlog/spdlog.h>

#include <net/tcp_server.h>


class Server : public net::TcpServer
{
  using net::TcpServer::TcpServer;

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
    constexpr uint16_t PORT = 13000;
    SPDLOG_INFO("starting server on {}", PORT);

    Server server(PORT);
    server.start();

    SPDLOG_INFO("server stopped", PORT);
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
