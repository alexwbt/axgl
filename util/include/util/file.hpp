#pragma once

#include <string>
#include <fstream>
#include <sstream>

namespace util
{

  inline std::string read_text_file(const std::string& path)
  {
    std::ifstream file(path, std::ifstream::in);
    if (!file.is_open())
      return "";

    std::stringstream output;
    output << file.rdbuf();
    return output.str();
  }

}
