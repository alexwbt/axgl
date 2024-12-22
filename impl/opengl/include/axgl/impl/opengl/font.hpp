#pragma once

#include <span>
#include <stdint.h>
#include <stdexcept>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <spdlog/spdlog.h>

#include <axgl/common.hpp>

NAMESPACE_AXGL_IMPL

class OpenglFontService : public interface::Service
{
private:
  FT_Library library_;
  std::unordered_map<std::string, FT_Face> faces_;

public:
  void initialize() override
  {
    if (FT_Init_FreeType(&library_))
      throw std::runtime_error("Failed to initialize freetype library.");
  }

  void load_typeface(const std::string& name, std::span<const uint8_t> buffer)
  {
    if (FT_New_Memory_Face(library_, buffer.data(), buffer.size(), 0, &faces_[name]))
      throw std::runtime_error("Failed to load fontface " + name);
  }
};

NAMESPACE_AXGL_IMPL_END
