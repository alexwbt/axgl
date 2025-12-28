#pragma once

#include <memory>
#include <string>
#include <vector>

#include <net/tcp.hpp>
#include <spdlog/spdlog.h>

inline std::shared_ptr<std::vector<uint8_t>> build_message(const std::string& message)
{
  return std::make_shared<std::vector<uint8_t>>(message.begin(), message.end());
}

inline void print_message(const std::vector<uint8_t>& buffer)
{
  constexpr int offset = net::LengthPrefixedTcpSocket::kLengthPrefixSize;
  const std::string_view value(reinterpret_cast<const char*>(buffer.data() + offset), buffer.size() - offset);
  AXGL_LOG_INFO(value);
}
