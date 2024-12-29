#pragma once

#include <span>
#include <memory>
#include <stdint.h>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

#include <utf8.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/static_vaos.hpp>
#include <opengl/static_shaders.hpp>

namespace opengl
{

  class FontCollection;

  class Character
  {
    Texture texture;
    glm::ivec2 size{ 0 };
    glm::ivec2 offset{ 0 };
    glm::ivec2 advance{ 0 };

    void load(FT_Face face, bool vertical)
    {
      const auto& glyph = face->glyph;
      const auto& bitmap = glyph->bitmap;

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      texture.load_texture(
        0, GL_RED,
        static_cast<GLint>(bitmap.width),
        static_cast<GLint>(bitmap.rows),
        0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
      texture.set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      texture.set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      size.x = texture.get_width();
      size.y = texture.get_height();
      offset.x = glyph->bitmap_left;
      offset.y = glyph->bitmap_top;
      advance.x = glyph->advance.x >> 6;
      advance.y = glyph->advance.y >> 6;
      
      SPDLOG_INFO(
        "size({},{}) "
        "offset({},{}) "
        "advance({},{}) ",
        size.x, size.y, offset.x, offset.y, advance.x, advance.y);
    }

    friend class Font;
  };

  class Font final
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

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    Font(Font&& other) noexcept
    {
      face_ = other.face_;
      other.face_ = nullptr;
    }
    Font& operator=(Font&& other) noexcept
    {
      if (this != &other)
      {
        if (face_)
          FT_Done_Face(face_);

        face_ = other.face_;
        other.face_ = 0;
      }
      return *this;
    }

    ~Font()
    {
      if (face_)
        FT_Done_Face(face_);
    }

    Texture render_text(const std::string& value, uint32_t size, bool vertical = false) const
    {
      std::unordered_map<uint32_t, Character> chars;

      FT_Set_Pixel_Sizes(face_, 0, size);

      int width = 0;
      int height = 0;
      glm::ivec2 max_offset(0);
      for (auto it = value.begin(), end = value.end(); it != end;)
      {
        uint32_t c = utf8::next(it, end);
        if (chars.contains(c))
          continue;

        if (FT_Load_Char(face_, c, FT_LOAD_RENDER))
        {
          SPDLOG_ERROR("Failed to load char '{}'", c);
          continue;
        }
        SPDLOG_INFO("{}", (char)c);
        chars[c].load(face_, vertical);

        if (vertical)
        {
          width = std::max(width, chars[c].size.x);
          height += chars[c].advance.y;
          max_offset.x = std::max(max_offset.x, chars[c].offset.x);
        }
        else
        {
          width += chars[c].advance.x;
          height = std::max(height, chars[c].size.y);
          max_offset.y = std::max(max_offset.y, chars[c].offset.y);
        }
      }

      Texture texture;
      texture.load_texture(0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
      texture.set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      texture.set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      Framebuffer framebuffer;
      framebuffer.attach_texture(0, texture);
      framebuffer.set_draw_buffers({ 0 });
      framebuffer.use();
      glViewport(0, 0, width, height);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glActiveTexture(GL_TEXTURE0);
      auto& shader = StaticShaders::instance().text();
      shader.use_program();
      shader.set_int("text_texture", 0);
      shader.set_vec3("text_color", glm::vec3(1));

      glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(width),
        0.0f, static_cast<float>(height));
      glm::vec3 advance(0);

      for (auto it = value.begin(), end = value.end(); it != end;)
      {
        uint32_t c = utf8::next(it, end);

        chars[c].texture.use();

        // this part is horizontal only for now
        glm::vec3 scale(chars[c].size, 1);
        glm::vec3 offset(chars[c].offset.x, chars[c].offset.y - chars[c].size.y/* + (max_offset.y - height)*/, 0);
        auto model = glm::translate(glm::mat4(1.0f), advance + offset) * glm::scale(scale);
        shader.set_mat4("mvp", projection * model);

        StaticVAOs::instance().quad().draw();

        if (vertical)
          advance.y += chars[c].advance.y;
        else
          advance.x += chars[c].advance.x;
      }

      glDisable(GL_BLEND);

      return texture;
    }
  };

  class FontCollection final
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
    FontCollection(const FontCollection&) = delete;
    FontCollection& operator=(const FontCollection&) = delete;

    FontCollection(FontCollection&& other) noexcept
    {
      fonts_ = std::move(other.fonts_);
      library_ = other.library_;
      other.library_ = nullptr;
    }
    FontCollection& operator=(FontCollection&& other) noexcept
    {
      if (this != &other)
      {
        fonts_.clear();
        if (library_)
          FT_Done_FreeType(library_);

        fonts_ = std::move(other.fonts_);
        library_ = other.library_;
        other.library_ = nullptr;
      }
      return *this;
    }

    ~FontCollection()
    {
      fonts_.clear();
      if (library_)
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

    bool has_font(const std::string& name) const
    {
      return fonts_.contains(name);
    }

    Texture render_text(
      const std::string& value,
      const std::string& font,
      uint32_t size) const
    {
      if (!fonts_.contains(font))
        throw std::runtime_error("FontCollection does not contain font: " + font);

      return fonts_.at(font)->render_text(value, size);
    }
  };

}
