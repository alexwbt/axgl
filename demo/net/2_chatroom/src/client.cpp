#include <format>
#include <iostream>
#include <memory>

#include <args.hxx>
#include <ftxui/component/screen_interactive.hpp>
#include <net/tcp/client.hpp>
#include <net/tcp/socket.hpp>

#include "fbs/message.hpp"
#include "tui/chat.hpp"

class Client final : public net::TcpClient
{
public:
  bool running = true;

  std::string username;
  std::shared_ptr<tui::Chat> chat_ui;

  explicit Client(const std::shared_ptr<asio::io_context>& io_context, std::string user) :
    net::TcpClient(io_context),
    username(std::move(user)),
    chat_ui(std::make_shared<tui::Chat>([&](const std::string& m) { on_input(m); }))
  {
  }

  std::shared_ptr<net::Socket> new_socket(asio::ip::tcp::socket socket) override
  {
    return std::make_shared<net::LengthPrefixedTcpSocket>(std::move(socket));
  }

  void connection_failed(const asio::error_code& code) override
  {
    chat_ui->add_message("Failed to connect to server: " + code.message());
    running = false;
  }

  void on_disconnect() override
  {
    chat_ui->add_message("Disconnected from server.");
    running = false;
  }

  void on_connect() override { chat_ui->add_message("Connected to server."); }

  void on_receive(const net::data_ptr_t& buffer) override
  {
    if (const auto* message = read_message(buffer))
      chat_ui->add_message(std::format("[{}] {}", message->sender()->str(), message->content()->str()));
  }

private:
  void on_input(const std::string& message)
  {
    if (message == "/clear")
    {
      chat_ui->clear_messages();
      return;
    }
    send(build_message(username, message));
  }
};

int main(int argc, char** argv)
{
  args::ArgumentParser parser("Net demo chatroom client.");
  args::ValueFlag<std::string> user_arg(parser, "user", "Username.", {'u', "user"}, "anon");
  args::ValueFlag<std::string> host_arg(parser, "host", "Server host.", {'h', "host"}, "127.0.0.1");
  args::ValueFlag<std::uint16_t> port_arg(parser, "port", "Server port.", {'p', "port"}, 10000);
  try
  {
    parser.ParseCLI(argc, argv);
    const auto& user = args::get(user_arg);
    const auto& host = args::get(host_arg);
    const auto& port = args::get(port_arg);

    const auto io_context = std::make_shared<asio::io_context>();
    Client client(io_context, user);

    std::thread io_thread([&]
    {
      try
      {
        io_context->run();
      }
      catch (const std::exception& e)
      {
        client.chat_ui->add_message(std::format("Error: {}", e.what()));
      }
    });

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    std::thread ui_thread([&]
    {
      screen.Loop(ftxui::Renderer(client.chat_ui, [&] { return client.chat_ui->Render(); }));
      client.disconnect();
    });

    client.chat_ui->add_message(std::format("Connecting to {}:{} as {}", host, port, user));
    client.connect(host, port);

    while (client.running)
      client.update();

    client.disconnect();
    screen.Exit();

    if (!io_context->stopped()) io_context->stop();
    if (ui_thread.joinable()) ui_thread.join();
    if (io_thread.joinable()) io_thread.join();
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
    std::cerr << e.what() << std::endl;
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
