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

  void on_connect(uint32_t session_id, SessionPtr session) override
  {
    SPDLOG_INFO("new connection (id: {})", session_id);

    // asio::error_code ignored_error;
    // asio::write(*socket, asio::buffer(get_daytime_string()), ignored_error);

    // remove_session(id);
  }

  void on_disconnect(uint32_t session_id) override
  {
    SPDLOG_INFO("client disconnected (id: {})", session_id);
  }

  void on_receive(uint32_t session_id, const std::string& message) override
  {
    SPDLOG_INFO("received message from client (id: {}): {}", session_id, message);
  }

};

int main()
{
  Server server(13000);
  server.start();
}
