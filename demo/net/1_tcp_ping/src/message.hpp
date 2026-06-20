#pragma once

#include <iostream>
#include <net/tcp/socket.hpp>

inline std::shared_ptr<std::vector<uint8_t>> build_message(const std::string& message)
{
  return std::make_shared<std::vector<uint8_t>>(message.begin(), message.end());
}

inline void print_message(const net::data_t& buffer)
{
  constexpr int kOffset = net::LengthPrefixedTcpSocket::kLengthPrefixSize;
  const std::string_view value(reinterpret_cast<const char*>(buffer.data() + kOffset), buffer.size() - kOffset);
  std::cout << value << std::endl;
}
