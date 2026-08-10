#pragma once

#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <util/numcast.hpp>

namespace util
{

inline std::string read_text_file(const std::string& path)
{
  const std::ifstream file(path, std::ifstream::in);
  if (!file.is_open()) throw std::runtime_error("Could not open file: " + path);

  std::stringstream output;
  output << file.rdbuf();
  return output.str();
}

inline std::vector<uint8_t> read_file(const std::string& path)
{
  std::ifstream file(path, std::ifstream::in);
  if (!file.is_open()) throw std::runtime_error("Could not open file: " + path);

  file.seekg(0, std::ios::end);
  const size_t size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(size);
  file.read(
    reinterpret_cast<char*>(buffer.data()),
    util::clamp_cast<std::streamsize>(size)
  );

  return buffer;
}

inline void write_text_file(const std::string& path, const std::string& content)
{
  std::ofstream file(path);
  if (!file.is_open()) throw std::runtime_error("Could not open file: " + path);

  file << content;
}

} // namespace util
