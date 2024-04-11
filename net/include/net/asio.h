#pragma once

#ifdef _WIN32
#include <SDKDDKVer.h>
#endif

#include <asio.hpp>

#include "net/namespace.h"

NAMESPACE_NET

class IoContextComponent
{
protected:
  std::shared_ptr<asio::io_context> io_context_;

  IoContextComponent(std::shared_ptr<asio::io_context> io_context) :
    io_context_(std::move(io_context)) {}

  virtual ~IoContextComponent() {}
};

NAMESPACE_NET_END
