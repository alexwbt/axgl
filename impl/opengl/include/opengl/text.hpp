#pragma once

#include <span>
#include <memory>
#include <stdint.h>
#include <stdexcept>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <spdlog/spdlog.h>
#include <utf8.h>


//#include <opengl/vertex_array_object.hpp>
//#include <opengl/shader_program.hpp>
#include <opengl/texture.hpp>

namespace opengl
{

  class FontCollection;

  //class Text
  //{
  //private:
  //  VertexArrayObject quad_vao_;
  //  Texture text_texture_;

  //public:
  //  Text(const std::string& value, uint32_t size)
  //  {
  //    
  //  }
  //};

  class Font
  {
  private:
    FT_Face face_;

  public:
    Font(FT_Library library, const std::string& path, int index)
    {
      if (FT_New_Face(library, path.c_str(), index, &face_))
        throw std::runtime_error("Failed to load fontface: " + path);
    }

    Font(FT_Library library, std::span<const uint8_t> buffer, int index)
    {
      if (FT_New_Memory_Face(library, buffer.data(), buffer.size(), index, &face_))
        throw std::runtime_error("Failed to load fontface from memory.");
    }

    ~Font()
    {
      FT_Done_Face(face_);
    }

    void create_text(const std::string& value) const
    {
      struct Character
      {
        Texture texture;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t bearing_x = 0;
        uint32_t bearing_y = 0;
        uint32_t advance_x = 0;
        uint32_t advance_y = 0;
      };
      std::unordered_map<uint32_t, Character> chars;
      uint32_t total_width = 0;
      uint32_t total_height = 0;

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      auto it = value.begin();
      auto end = value.end();
      while (it != end)
      {
        uint32_t c = utf8::next(it, end);

        if (chars.contains(c))
          continue;

        if (FT_Load_Char(face_, c, FT_LOAD_RENDER))
        {
          SPDLOG_ERROR("Failed to load char '{}'", c);
          continue;
        }

        chars[c].texture.load_raw_texture_2d({
          0, GL_RED,
          static_cast<GLint>(face_->glyph->bitmap.width),
          static_cast<GLint>(face_->glyph->bitmap.rows),
          0, GL_RED, GL_UNSIGNED_BYTE, face_->glyph->bitmap.buffer, });
        chars[c].texture.set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        chars[c].texture.set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        chars[c].texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        chars[c].texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        chars[c].width = face_->glyph->bitmap.width;
        chars[c].height = face_->glyph->bitmap.rows;
        chars[c].bearing_x = face_->glyph->bitmap_left;
        chars[c].bearing_y = face_->glyph->bitmap_top;
        chars[c].advance_x = face_->glyph->advance.x;
        chars[c].advance_y = face_->glyph->advance.y;
      }
    }
  };

  class FontCollection
  {
  private:
    FT_Library library_;
    std::unordered_map<std::string, std::unique_ptr<Font>> fonts_;

  public:
    FontCollection()
    {
      if (FT_Init_FreeType(&library_))
        throw std::runtime_error("Failed to initialize freetype library.");
    }

    ~FontCollection()
    {
      fonts_.clear();
      FT_Done_FreeType(library_);
    }

    void load_font(const std::string& name, const std::string& path, int index = 0)
    {
      auto font = std::make_unique<Font>(library_, path, index);
      fonts_[name] = std::move(font);
    }

    void load_font(const std::string& name, std::span<const uint8_t> buffer, int index = 0)
    {
      auto font = std::make_unique<Font>(library_, buffer, index);
      fonts_[name] = std::move(font);
    }

    void unload_font(const std::string& name)
    {
      fonts_.erase(name);
    }
  };

}
