#include <iostream>
#include <net/tcp/client.hpp>

#include "message.hpp"

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
    std::cout << error_code.message() << std::endl;
  }

  void on_disconnect() override { }

  void on_connect() override { send(build_message("hello world")); }

  void on_receive(const net::data_ptr_t buffer) override
  {
    print_message(*buffer);
    stop = true;
  }
};

int main()
{
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
        std::cerr << e.what() << std::endl;
      }
    });

    client.connect("127.0.0.1", 10000);

    while (!client.stop)
      client.update();

    client.disconnect();

    if (!io_context->stopped()) io_context->stop();

    if (thread.joinable()) thread.join();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
