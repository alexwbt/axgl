#pragma once

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace util
{

template <typename result_t>
void split(const std::string& s, const char delim, result_t result)
{
  std::istringstream iss(s);
  std::string item;
  while (std::getline(iss, item, delim))
    *result++ = item;
}

inline std::vector<std::string> split(const std::string& s, char delim)
{
  std::vector<std::string> tokens;
  split(s, delim, std::back_inserter(tokens));
  return tokens;
}

// trim from start (in place)
inline void ltrim(std::string& value)
{
  const auto char_is_space = [](const unsigned char ch) { return !std::isspace(ch); };
  value.erase(value.begin(), std::ranges::find_if(value, char_is_space));
}

// trim from end (in place)
inline void rtrim(std::string& value)
{
  value.erase(
    std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), value.end());
}

// trim from both ends (in place)
inline void trim(std::string& value)
{
  rtrim(value);
  ltrim(value);
}

// trim from start (copying)
inline std::string ltrim_copy(std::string value)
{
  ltrim(value);
  return value;
}

// trim from end (copying)
inline std::string rtrim_copy(std::string value)
{
  rtrim(value);
  return value;
}

// trim from both ends (copying)
inline std::string trim_copy(std::string value)
{
  trim(value);
  return value;
}

constexpr std::size_t hash_string(const char* str, const std::size_t hash = 5381)
{
  return (*str == '\0') ? hash : hash_string(str + 1, (hash * 33) ^ static_cast<std::size_t>(*str));
}

constexpr std::size_t hash_string(const std::string& value) { return hash_string(value.c_str()); }

inline void string_to_vec3(const std::string& value, float& x, float& y, float& z)
{
  std::istringstream iss(value);
  if (char comma; !(iss >> x >> comma >> y >> comma >> z))
  {
    x = 0;
    y = 0;
    z = 0;
  }
}

inline int string_to_int(const std::string& value)
{
  const char* str = value.c_str();
  char* end = nullptr;
  errno = 0;
  const long val = std::strtol(str, &end, 10);
  if (end == str) throw std::invalid_argument("string_to_int: no digits found");
  if (errno == ERANGE) throw std::out_of_range("string_to_int: value out of range for long");
  if (*end != '\0') throw std::invalid_argument("string_to_int: trailing characters");
  if (val < INT_MIN || val > INT_MAX) throw std::out_of_range("string_to_int: value out of range for int");
  return static_cast<int>(val);
}

} // namespace util
