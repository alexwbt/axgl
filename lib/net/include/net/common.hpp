#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#ifdef _WIN32
  #include <SDKDDKVer.h>
#endif

#include <asio.hpp>

namespace net
{

typedef const std::vector<std::uint8_t> data_t;
typedef std::shared_ptr<data_t> data_ptr_t;

} // namespace net
