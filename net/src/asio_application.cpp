#include "net/asio_application.h"

namespace net
{

  void AsioApplication::async_run(asio::awaitable<void> task)
  {
    asio::co_spawn(io_context_, std::move(task), asio::detached);
  }

}
