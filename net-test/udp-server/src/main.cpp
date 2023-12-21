#include <chrono>

#include <spdlog/spdlog.h>
#include <net/asio.h>

using asio::ip::udp;

std::string get_daytime_string()
{
  const auto now = std::chrono::system_clock::now();
  const auto time = std::chrono::system_clock::to_time_t(now);

  char buf[20];
  strftime(buf, 20, "%d.%m.%Y %H:%M:%S", localtime(&time));
  return { buf };
}

int main()
{
  try
  {
    constexpr uint16_t PORT = 13000;
    SPDLOG_INFO("starting server on {}", PORT);

    asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), PORT));

    while (true)
    {
      std::vector<char> recv_buf(1);
      udp::endpoint remote_endpoint;
      socket.receive_from(asio::buffer(recv_buf), remote_endpoint);
      SPDLOG_INFO("Received request from {}", remote_endpoint.address().to_string());

      std::string message = get_daytime_string();
      asio::error_code ignored_error;
      socket.send_to(asio::buffer(message),
        remote_endpoint, 0, ignored_error);
    }

    SPDLOG_INFO("server stopped");
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
