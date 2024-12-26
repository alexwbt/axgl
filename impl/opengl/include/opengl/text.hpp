#pragma once

#include <span>
#include <memory>
#include <stdint.h>
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

#define FOR_EACH_UTF8_CHAR(str, c)                        \
uint32_t c;                                               \
for (                                                     \
auto _utf8_char_iterator_ = str.begin(), end = str.end(); \
_utf8_char_iterator_ != end;                              \
c = utf8::next(_utf8_char_iterator_, end)                 \
)

namespace opengl
{

  class FontCollection;

  //class Text
  //{
  //private:
  //  VertexArrayObject quad_vao_;
  //  Texture text_texture_;

  //public:
  //  Text(const std::string& value)
  //  {
  //    
  //  }
  //};

  class Character
  {
    Texture texture;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t bearing_x = 0;
    uint32_t bearing_y = 0;
    uint32_t advance_x = 0;
    uint32_t advance_y = 0;

    void load(FT_Face face)
    {
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      texture.load_texture(
        0, GL_RED,
        static_cast<GLint>(face->glyph->bitmap.width),
        static_cast<GLint>(face->glyph->bitmap.rows),
        0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
      texture.set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      texture.set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      width = face->glyph->bitmap.width;
      height = face->glyph->bitmap.rows;
      bearing_x = face->glyph->bitmap_left;
      bearing_y = face->glyph->bitmap_top;
      advance_x = face->glyph->advance.x;
      advance_y = face->glyph->advance.y;
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

    Font(Font&& other)
    {
      face_ = other.face_;
      other.face_ = nullptr;
    }
    Font& operator=(Font&& other)
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

    void create_text(const std::string& value, uint32_t size) const
    {
      std::unordered_map<uint32_t, Character> chars;
      uint32_t total_width = 0;

      FT_Set_Pixel_Sizes(face_, 0, size);

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
        chars[c].load(face_);
        total_width += chars[c].width;
      }

      Texture texture;
      texture.load_texture(0, GL_RGB, total_width, size, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

      Framebuffer framebuffer;
      framebuffer.attach_texture(0, texture);
      framebuffer.set_draw_buffers({ 0 });
      framebuffer.use();
      glViewport(0, 0, total_width, size);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      auto& shader = StaticShaders::instance().mesh_2d();
      shader.use_program();
      glActiveTexture(GL_TEXTURE0);
      shader.set_int("mesh_texture", 0);
      shader.set_bool("use_texture", true);

      glm::vec2 v = glm::vec2(total_width, size) * 0.5f;
      glm::mat4 mat = glm::ortho(v.x, -v.x, -v.y, v.y);

      for (auto it = value.begin(), end = value.end(); it != end;)
      {
        uint32_t c = utf8::next(it, end);

        chars[c].texture.use();
        shader.set_mat4("mvp", mat);

        StaticVAOs::instance().quad().draw();
        mat = glm::translate(mat, glm::vec3(chars[c].advance_x, 0, 0));
      }
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

    FontCollection(FontCollection&& other)
    {
      fonts_ = std::move(other.fonts_);
      library_ = other.library_;
      other.library_ = nullptr;
    }
    FontCollection& operator=(FontCollection&& other)
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
  };

}
