#include <common/server.h>


int main()
{
  try
  {
    Server server(13000);

    std::thread thread([&]()
    {
      server.start();
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
