#pragma once

#include <span>
#include <memory>
#include <format>
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

  class Font;
  class TextRenderer;

  struct TextOptions final
  {
    uint32_t size = 0;
    bool vertical = false;
  };

  struct Text final
  {
    Texture texture;
    glm::ivec2 size{ 0 };
    glm::vec2 offset{ 0 };
  };

  class Character final
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
        static_cast<GLsizei>(bitmap.width),
        static_cast<GLsizei>(bitmap.rows),
        0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
      texture.set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      texture.set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      size.x = texture.get_width();
      size.y = texture.get_height();
      offset.x = glyph->bitmap_left;
      offset.y = glyph->bitmap_top - size.y;
      advance.x = glyph->advance.x >> 6;
      advance.y = glyph->advance.y >> 6;
    }

    friend class Font;
    friend class TextRenderer;
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

    void load_char(Character& character, uint32_t code, const TextOptions& options) const
    {
      FT_Set_Pixel_Sizes(face_, 0, options.size);
      if (FT_Load_Char(face_, code, FT_LOAD_RENDER))
      {
        SPDLOG_ERROR("Failed to load char {}", code);
        return;
      }
      character.load(face_, options.vertical);
    }

    bool has_char(uint32_t code) const
    {
      return FT_Get_Char_Index(face_, code) > 0;
    }
  };

  class TextRenderer final
  {
  private:
    FT_Library library_;
    std::unordered_map<std::string, std::unique_ptr<Font>> fonts_;

  public:
    TextRenderer()
    {
      if (FT_Init_FreeType(&library_))
        throw std::runtime_error("Failed to initialize freetype library.");
    }
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    TextRenderer(TextRenderer&& other) noexcept
    {
      fonts_ = std::move(other.fonts_);
      library_ = other.library_;
      other.library_ = nullptr;
    }
    TextRenderer& operator=(TextRenderer&& other) noexcept
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

    ~TextRenderer()
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

    int get_renderable_font(const std::vector<std::string>& font, uint32_t c) const
    {
      for (int i = 0; i < font.size(); ++i)
        if (has_font(font[i]) && fonts_.at(font[i])->has_char(c))
          return i;
      return -1;
    }

    void render_text(
      Text& target,
      const std::string& value,
      const std::vector<std::string>& font,
      TextOptions options
    ) const
    {
      std::unordered_map<uint32_t, Character> chars;

      int width = 0;
      int height = 0;
      glm::ivec2 min_offset(0);
      for (auto it = value.begin(), end = value.end(); it != end;)
      {
        uint32_t c = utf8::next(it, end);
        if (!chars.contains(c))
        {
          int f = get_renderable_font(font, c);
          if (f < 0)
          {
            SPDLOG_ERROR("Unrenderable char: {} (decimal code point)", c);
            continue;
          }
          fonts_.at(font[f])->load_char(chars[c], c, options);
        }

        if (options.vertical)
        {
          width = std::max(width, chars[c].size.x);
          height += chars[c].advance.y;
        }
        else
        {
          width += chars[c].advance.x;
          height = std::max(height, chars[c].size.y);
        }
        min_offset.x = std::min(min_offset.x, chars[c].offset.x);
        min_offset.y = std::min(min_offset.y, chars[c].offset.y);
      }
      width -= min_offset.x;
      height -= min_offset.y;

      target.size = glm::ivec2(width, height);
      target.texture.load_texture(0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
      target.texture.set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      target.texture.set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      target.texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      target.texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      Framebuffer framebuffer;
      framebuffer.attach_texture(0, target.texture);
      framebuffer.set_draw_buffers({ 0 });
      framebuffer.use();
      glViewport(0, 0, width, height);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glActiveTexture(GL_TEXTURE0);
      auto shader = StaticShaders::instance().text();
      shader->use_program();
      shader->set_int("text_texture", 0);
      shader->set_vec4("text_color", glm::vec4(1));

      glm::vec3 advance(0);
      glm::mat4 projection = glm::ortho(
        static_cast<float>(width), 0.0f,
        static_cast<float>(height), 0.0f);
      auto quad = StaticVAOs::instance().quad();

      for (auto it = value.begin(), end = value.end(); it != end;)
      {
        uint32_t c = utf8::next(it, end);
        if (!chars.contains(c))
          continue;

        chars[c].texture.use();
        glm::vec3 scale(chars[c].size, 1);
        glm::vec3 offset(chars[c].offset - min_offset, 0);
        auto model = glm::translate(glm::mat4(1.0f), advance + offset) * glm::scale(scale);
        shader->set_mat4("mvp", projection * model);

        quad->draw();

        if (options.vertical)
          advance.y += chars[c].advance.y;
        else
          advance.x += chars[c].advance.x;
      }

      glDisable(GL_BLEND);
    }
  };

}
