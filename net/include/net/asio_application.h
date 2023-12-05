#pragma once

#ifdef _WIN32
#include <SDKDDKVer.h>
#endif

#include <asio.hpp>

namespace net
{

  class AsioApplication
  {
  protected:
    asio::io_context io_context_;

  public:
    AsioApplication() {}
    virtual ~AsioApplication() {}

    void async_run(asio::awaitable<void> task);
  };

}
