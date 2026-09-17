#pragma once

#include <algorithm>
#include <cstdint>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <unordered_map>

#include <util/numcast.hpp>

#include <ft2build.h>
#include <utf8.h>
#include FT_FREETYPE_H
#include <axgl/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/shader_program.hpp>
#include <opengl/static_shaders.hpp>
#include <opengl/static_vaos.hpp>
#include <opengl/texture.hpp>

namespace opengl {

class Font;
class TextRenderer;

enum class WrapMode {
  None,
  Word,
  Char,
  Auto,
};

struct TextOptions final {
  glm::vec4 color{1};
  std::uint32_t size = 0;
  std::int32_t max_width = -1;
  std::int32_t max_height = -1;
  float line_height = 0.0f;
  WrapMode wrap = WrapMode::Auto;
  bool vertical = false;
};

namespace text_detail {

[[nodiscard]] inline bool is_cjk_char(std::uint32_t c) noexcept {
  return (c >= 0x1100 && c <= 0x11FF) || (c >= 0x2E80 && c <= 0x2FFF)
    || (c >= 0x3000 && c <= 0x303F) || (c >= 0x3040 && c <= 0x309F)
    || (c >= 0x30A0 && c <= 0x30FF) || (c >= 0x3100 && c <= 0x312F)
    || (c >= 0x3130 && c <= 0x318F) || (c >= 0x3400 && c <= 0x4DBF)
    || (c >= 0x4E00 && c <= 0x9FFF) || (c >= 0xA000 && c <= 0xA4CF)
    || (c >= 0xAC00 && c <= 0xD7AF) || (c >= 0xF900 && c <= 0xFAFF)
    || (c >= 0xFE30 && c <= 0xFE4F) || (c >= 0xFF00 && c <= 0xFFEF)
    || (c >= 0x1F000 && c <= 0x1FAFF) || (c >= 0x20000 && c <= 0x2FFFD)
    || (c >= 0x30000 && c <= 0x3FFFD);
}

[[nodiscard]] inline bool is_word_boundary_char(std::uint32_t c) noexcept {
  return c == 0x20 || c == 0x09 || c == 0x0A || c == 0x0D
    || (c >= 0x2000 && c <= 0x200A) || c == 0x2028 || c == 0x2029;
}

[[nodiscard]] inline bool is_no_break_before_char(std::uint32_t c) noexcept {
  if (
    c == 0x0027 || c == 0x002C || c == 0x002E || c == 0x003A || c == 0x003B
    || c == 0x0021 || c == 0x003F || c == 0x0029 || c == 0x005D || c == 0x007D
  )
    return true;
  if (
    (c >= 0x0300 && c <= 0x036F) || (c >= 0x1AB0 && c <= 0x1AFF)
    || (c >= 0x1DC0 && c <= 0x1DFF) || (c >= 0x20D0 && c <= 0x20FF)
    || (c >= 0xFE20 && c <= 0xFE2F)
  )
    return true;
  if (c == 0x200D || c == 0x200C || c == 0xFEFF) return true;
  if (
    c == 0x2018 || c == 0x2019 || c == 0x201C || c == 0x201D || c == 0x2032
    || c == 0x2033 || c == 0x3001 || c == 0x3002 || c == 0x300D || c == 0x300F
    || c == 0x3011 || c == 0x3015 || c == 0xFF01 || c == 0xFF0C || c == 0xFF0E
    || c == 0xFF1A || c == 0xFF1B || c == 0xFF1F || c == 0xFF3D || c == 0xFF5D
  )
    return true;
  return false;
}

[[nodiscard]] inline bool can_break_before(
  std::uint32_t prev, std::uint32_t curr, WrapMode mode
) noexcept {
  if (mode == WrapMode::None) return false;
  if (mode == WrapMode::Char) return true;
  if (is_word_boundary_char(curr) || is_word_boundary_char(prev)) return true;
  if (is_no_break_before_char(curr)) return false;
  const bool prev_cjk = is_cjk_char(prev);
  const bool curr_cjk = is_cjk_char(curr);
  if (mode == WrapMode::Auto && (prev_cjk || curr_cjk)) {
    if (prev_cjk && curr_cjk) return true;
    if (prev_cjk && !is_word_boundary_char(curr)) return true;
    if (curr_cjk && !is_word_boundary_char(prev)) return true;
    return false;
  }
  return false;
}

} // namespace text_detail

struct Text final {
  Texture texture;
  glm::ivec2 size{0};
  glm::vec2 offset{0};
};

class Character final {
  Texture texture;
  glm::ivec2 size{0};
  glm::ivec2 offset{0};
  glm::ivec2 advance{0};

  void load(FT_Face face, bool) {
    const auto& glyph = face->glyph;
    const auto& bitmap = glyph->bitmap;

    size.x = util::narrow<int>(bitmap.width);
    size.y = util::narrow<int>(bitmap.rows);
    if (bitmap.width > 0 && bitmap.rows > 0) {
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      texture.load_texture(
        0, GL_RED, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer
      );
      texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    offset.x = glyph->bitmap_left;
    offset.y = glyph->bitmap_top - size.y;
    advance.x = glyph->advance.x >> 6;
    advance.y = glyph->advance.y >> 6;
  }

  friend class Font;
  friend class TextRenderer;
};

class Font final {
  FT_Face face_;

public:
  Font(FT_Library library, const std::string& path, const int index) {
    if (FT_New_Face(library, path.c_str(), index, &face_))
      throw std::runtime_error("Failed to load fontface: " + path);
  }

  Font(
    FT_Library library, const std::span<const uint8_t> buffer, const int index
  ) {
    if (
      FT_New_Memory_Face(
        library,
        buffer.data(),
        util::narrow<FT_Long>(buffer.size()),
        index,
        &face_
      )
    )
      throw std::runtime_error("Failed to load font face from memory.");
  }

  Font(const Font&) = delete;
  Font& operator=(const Font&) = delete;

  Font(Font&& other) noexcept {
    face_ = other.face_;
    other.face_ = nullptr;
  }
  Font& operator=(Font&& other) noexcept {
    if (this != &other) {
      if (face_) FT_Done_Face(face_);

      face_ = other.face_;
      other.face_ = nullptr;
    }
    return *this;
  }

  ~Font() {
    if (face_) FT_Done_Face(face_);
  }

  void load_char(
    Character& character, uint32_t code, const TextOptions& options
  ) const {
    FT_Set_Pixel_Sizes(face_, 0, options.size);
    if (FT_Load_Char(face_, code, FT_LOAD_RENDER)) {
      AXGL_LOG_ERROR("Failed to load char {}", code);
      return;
    }
    character.load(face_, options.vertical);
  }

  [[nodiscard]] bool has_char(uint32_t code) const {
    return FT_Get_Char_Index(face_, code) > 0;
  }
};

class TextRenderer final {
  FT_Library library_;
  std::unordered_map<std::string, std::unique_ptr<Font>> fonts_;

public:
  TextRenderer() {
    if (FT_Init_FreeType(&library_))
      throw std::runtime_error("Failed to initialize freetype library.");
  }
  TextRenderer(const TextRenderer&) = delete;
  TextRenderer& operator=(const TextRenderer&) = delete;

  TextRenderer(TextRenderer&& other) noexcept {
    fonts_ = std::move(other.fonts_);
    library_ = other.library_;
    other.library_ = nullptr;
  }
  TextRenderer& operator=(TextRenderer&& other) noexcept {
    if (this != &other) {
      fonts_.clear();
      if (library_) FT_Done_FreeType(library_);

      fonts_ = std::move(other.fonts_);
      library_ = other.library_;
      other.library_ = nullptr;
    }
    return *this;
  }

  ~TextRenderer() {
    fonts_.clear();
    if (library_) FT_Done_FreeType(library_);
  }

  void load_font(
    const std::string& name, const std::string& path, int index = 0
  ) {
    auto font = std::make_unique<Font>(library_, path, index);
    fonts_[name] = std::move(font);
  }

  void load_font(
    const std::string& name, std::span<const uint8_t> buffer, int index = 0
  ) {
    auto font = std::make_unique<Font>(library_, buffer, index);
    fonts_[name] = std::move(font);
  }

  void unload_font(const std::string& name) { fonts_.erase(name); }

  [[nodiscard]] bool has_font(const std::string& name) const {
    return fonts_.contains(name);
  }

  [[nodiscard]] int get_renderable_font(
    const std::vector<std::string>& font, std::uint32_t c
  ) const {
    const int size = util::clamp_cast<int>(font.size());
    for (int i = 0; i < size; ++i)
      if (has_font(font[i]) && fonts_.at(font[i])->has_char(c)) return i;
    return -1;
  }

  void render_text(
    Text& target,
    const std::string& value,
    const std::vector<std::string>& font,
    const TextOptions& options
  ) const {
    if (value.empty()) {
      target.size = glm::ivec2(0);
      return;
    }

    std::unordered_map<std::uint32_t, Character> chars;
    std::vector<std::uint32_t> codepoints;
    codepoints.reserve(value.size() / 2);
    for (auto it = value.begin(), end = value.end(); it != end;) {
      std::uint32_t c = utf8::next(it, end);
      if (!chars.contains(c)) {
        int f = get_renderable_font(font, c);
        if (f < 0) {
          AXGL_LOG_ERROR("Unrenderable char: {} (decimal code point)", c);
          continue;
        }
        fonts_.at(font[f])->load_char(chars[c], c, options);
      }
      codepoints.push_back(c);
    }
    if (codepoints.empty()) {
      target.size = glm::ivec2(0);
      return;
    }

    glm::ivec2 min_offset(0);
    for (const auto& [c, ch] : chars) {
      (void)c;
      min_offset.x = std::min(min_offset.x, ch.offset.x);
      min_offset.y = std::min(min_offset.y, ch.offset.y);
    }

    const int line_step = options.line_height > 0.0f
      ? util::clamp_cast<int>(options.line_height)
      : util::clamp_cast<int>(options.size);

    const bool do_wrap = options.wrap != WrapMode::None
      && ((options.vertical && options.max_height > 0)
          || (!options.vertical && options.max_width > 0));

    struct Placed {
      std::uint32_t c;
      int x;
      int y;
    };
    std::vector<Placed> placed;

    int max_primary = 0;
    int max_secondary = 0;
    int total_secondary = 0;

    if (!do_wrap) {
      int pen_a = 0;
      for (const auto c : codepoints) {
        const auto& ch = chars[c];
        if (options.vertical) {
          placed.push_back({c, 0, pen_a});
          pen_a += ch.advance.y;
          max_secondary = std::max(max_secondary, ch.size.x);
        } else {
          placed.push_back({c, pen_a, 0});
          pen_a += ch.advance.x;
          max_secondary = std::max(max_secondary, ch.size.y);
        }
      }
      max_primary = pen_a;
      total_secondary = max_secondary;
    } else {
      int pen_a = 0;
      int pen_b = 0;
      int line_start = 0;
      int last_break = -1;

      const auto flush_line = [&]() {
        max_primary = std::max(max_primary, pen_a);
        pen_a = 0;
        pen_b += line_step;
        line_start = static_cast<int>(placed.size());
        last_break = -1;
      };

      for (std::size_t i = 0; i < codepoints.size(); ++i) {
        const auto c = codepoints[i];
        const auto& ch = chars[c];
        const int advance = options.vertical ? ch.advance.y : ch.advance.x;
        const int max_extent
          = options.vertical ? options.max_height : options.max_width;

        if (
          pen_a + advance > max_extent && pen_a > 0
          && !text_detail::is_no_break_before_char(c)
        ) {
          if (last_break > line_start) {
            std::vector<Placed> carried(
              placed.begin() + last_break, placed.end()
            );
            placed.resize(static_cast<std::size_t>(last_break));
            flush_line();
            for (auto& p : carried) {
              const int& adv = options.vertical ? chars[p.c].advance.y
                                                : chars[p.c].advance.x;
              if (options.vertical) {
                p.x = pen_b;
                p.y = pen_a;
              } else {
                p.x = pen_a;
                p.y = pen_b;
              }
              pen_a += adv;
              placed.push_back(p);
            }
          } else {
            flush_line();
          }
        }

        if (options.vertical) placed.push_back({c, pen_b, pen_a});
        else placed.push_back({c, pen_a, pen_b});
        pen_a += advance;
        max_secondary
          = std::max(max_secondary, options.vertical ? ch.size.x : ch.size.y);

        if (i + 1 < codepoints.size()) {
          const auto next = codepoints[i + 1];
          if (text_detail::can_break_before(c, next, options.wrap))
            last_break = static_cast<int>(placed.size());
        }
      }
      max_primary = std::max(max_primary, pen_a);
      total_secondary = pen_b + max_secondary;
    }

    int width = options.vertical ? total_secondary : max_primary;
    int height = options.vertical ? max_primary : total_secondary;
    width -= min_offset.x;
    height -= min_offset.y;

    if (do_wrap) {
      if (options.max_width > 0 && width > options.max_width)
        width = options.max_width;
      if (options.max_height > 0 && height > options.max_height)
        height = options.max_height;
    }

    if (width <= 0 || height <= 0) {
      target.size = glm::ivec2(0);
      return;
    }

    target.size = glm::ivec2(width, height);
    target.texture.load_texture(
      0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    target.texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    target.texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    target.texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    target.texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const Framebuffer framebuffer;
    framebuffer.attach_texture(GL_COLOR_ATTACHMENT0, target.texture);
    framebuffer.set_draw_buffers({GL_COLOR_ATTACHMENT0});
    framebuffer.check_status_complete();
    framebuffer.use();
    glViewport(0, 0, width, height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto& text_shader = ::opengl::StaticShaders::instance().text();
    text_shader.use_program();
    text_shader.set_int("text_texture", 0);
    text_shader.set_bool("use_instancing", false);
    text_shader.set_vec4("text_color", options.color);

    glm::mat4 projection = glm::ortho(
      static_cast<float>(width), 0.0f, static_cast<float>(height), 0.0f
    );
    auto& quad = StaticVAOs::instance().quad();

    for (const auto& [c, px, py] : placed) {
      if (px >= width || py >= height) continue;
      const auto& ch = chars[c];
      if (ch.texture.initialized()) {
        ch.texture.use(GL_TEXTURE0);
        glm::vec3 scale(ch.size, 1.0f);
        glm::vec3 offset(
          ch.offset.x - min_offset.x + px, ch.offset.y - min_offset.y + py, 0.0f
        );
        auto model
          = glm::translate(glm::mat4(1.0f), offset) * glm::scale(scale);
        text_shader.set_mat4("projection_view_model", projection * model);

        quad.draw();
      }
    }

    glDisable(GL_BLEND);
  }
};

} // namespace opengl
