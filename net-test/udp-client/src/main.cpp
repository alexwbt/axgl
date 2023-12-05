#include <spdlog/spdlog.h>
#include <sstream>

#include <net/asio_application.h>

using asio::ip::udp;


int main()
{
  try
  {
    asio::io_context io_context;
    udp::resolver resolver(io_context);

    udp::socket socket(io_context);
    socket.open(udp::v4());

    auto receiver_endpoint = *resolver.resolve(udp::v4(), "localhost", "13000").begin();

    std::vector<char> send_buf = { 0 };
    socket.send_to(asio::buffer(send_buf), receiver_endpoint);

    // receive
    std::stringstream stringstream;

    std::vector<char> recv_buf(128);
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(asio::buffer(recv_buf), sender_endpoint);
    stringstream.write(recv_buf.data(), len);

    SPDLOG_INFO("Received daytime: {}", stringstream.str());
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
