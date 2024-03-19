#include "server.h"

int main(int argc, char* argv[])
{
  try
  {
    int port = argc > 1 ? std::stoi(argv[1]) : 3000;

    auto io_context = std::make_shared<asio::io_context>();
    NetServer server(io_context, port);
    server.start();

    std::thread thread([&]()
    {
      try { io_context->run(); }
      catch (const std::exception& e)
      { SPDLOG_CRITICAL(e.what()); }
    });

    while (server.running())
      server.update();

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
