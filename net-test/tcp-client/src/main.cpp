#include <spdlog/spdlog.h>
#include <sstream>

#include <net/asio.h>

using asio::ip::tcp;


int main()
{
  try
  {
    asio::io_context io_context;
    tcp::resolver resolver(io_context);

    tcp::socket socket(io_context);
    asio::connect(socket, resolver.resolve("localhost", "13000"));

    SPDLOG_INFO("Connected");


    asio::error_code ignored_error;
    asio::write(socket, asio::buffer("hello world\n"), ignored_error);

    // std::stringstream stringstream;
    // while (true)
    // {
    //   std::vector<char> buf(128);
    //   asio::error_code error;

    //   size_t len = socket.read_some(asio::buffer(buf), error);
    //   if (error == asio::error::eof)
    //     break; // Connection closed cleanly by peer.
    //   else if (error)
    //     throw asio::system_error(error); // Some other error.

    //   stringstream.write(buf.data(), len);
    // }

    // SPDLOG_INFO("Received daytime: {}", stringstream.str());
  }
  catch (const std::exception& e)
  {
    SPDLOG_CRITICAL(e.what());
  }
}
