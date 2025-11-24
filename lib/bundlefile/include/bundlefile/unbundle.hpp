#pragma once

#include <string>
#include <fstream>
#include <cstdint>

#include <bundlefile/bundle_fbs.h>

namespace bundlefile
{

  class Bundle
  {
    std::vector<uint8_t> data_;

  public:
    Bundle(const std::string& path)
    {
      std::ifstream input_stream(path, std::ios::binary | std::ios::ate);
      if (!input_stream)
        throw std::runtime_error("Failed to open bundlefile: " + path);

      const auto size = input_stream.tellg();
      input_stream.seekg(0, std::ios::beg);

      data_.resize(size);
      input_stream.read(reinterpret_cast<char*>(data_.data()), size);
    }

    [[nodiscard]] const auto* get_bundle() const
    {
      return fbs::bundlefile::GetBundle(data_.data());
    }
  };

}
