#pragma once

#include <axgl/common.hpp>
#include <net/tcp.hpp>

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
