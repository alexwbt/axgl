#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace util
{

template <typename result_t> inline void split(const std::string& s, char delim, result_t result)
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
inline void ltrim(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
inline void rtrim(std::string& s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string& s)
{
  rtrim(s);
  ltrim(s);
}

// trim from start (copying)
inline std::string ltrim_copy(std::string s)
{
  ltrim(s);
  return s;
}

// trim from end (copying)
inline std::string rtrim_copy(std::string s)
{
  rtrim(s);
  return s;
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s)
{
  trim(s);
  return s;
}

constexpr std::size_t hash_string(const char* str, std::size_t hash = 5381)
{
  return (*str == '\0') ? hash : hash_string(str + 1, (hash * 33) ^ static_cast<std::size_t>(*str));
}

constexpr std::size_t hash_string(const std::string& str) { return hash_string(str.c_str()); }

void string_to_vec3(const std::string& str, float& x, float& y, float& z)
{
  std::istringstream iss(str);
  char comma;
  if (!(iss >> x >> comma >> y >> comma >> z))
  {
    x = 0;
    y = 0;
    z = 0;
  }
}

} // namespace util
