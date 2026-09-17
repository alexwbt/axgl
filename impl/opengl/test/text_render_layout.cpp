#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "gl_context.hpp"
#include <opengl/text.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

namespace
{

const std::vector<std::uint8_t>& font_data()
{
  static const auto data = []
  {
    const auto font_path
      = std::filesystem::path(__FILE__).parent_path() / "res" / "arial.ttf";
    std::ifstream file(font_path, std::ios::binary);
    return std::vector<std::uint8_t>(
      std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()
    );
  }();
  return data;
}

const std::vector<std::string> fonts{"arial"};

opengl::TextOptions base_options()
{
  opengl::TextOptions opts;
  opts.size = 32;
  opts.color = {1, 1, 1, 1};
  return opts;
}

int single_line_width(opengl::TextRenderer& renderer, const std::string& text)
{
  auto opts = base_options();
  opts.wrap = opengl::WrapMode::None;
  opengl::Text t;
  renderer.render_text(t, text, fonts, opts);
  return t.size.x;
}

bool skip_if_no_context_or_font(const GlContext& gl)
{
  if (!gl.available())
  {
    MESSAGE("skipped: no GL context available");
    return true;
  }
  if (font_data().empty())
  {
    MESSAGE("skipped: test font not found");
    return true;
  }
  return false;
}

} // namespace

TEST_SUITE("opengl::TextRenderer::render_text layout")
{
  TEST_CASE("short text does not wrap and fits max_width")
  {
    const GlContext gl;
    if (skip_if_no_context_or_font(gl)) return;

    opengl::TextRenderer renderer;
    renderer.load_font("arial", font_data(), 0);

    const std::string text = "hello";
    const auto opts = base_options();
    const int unwrapped = single_line_width(renderer, text);

    opengl::Text t;
    auto wrap_opts = opts;
    wrap_opts.max_width = unwrapped + 1000;
    wrap_opts.wrap = opengl::WrapMode::Word;
    renderer.render_text(t, text, fonts, wrap_opts);

    CHECK(t.size.x == unwrapped);
    CHECK(t.size.y > 0);
  }

  TEST_CASE("Word wrap: long text wraps and height grows")
  {
    const GlContext gl;
    if (skip_if_no_context_or_font(gl)) return;

    opengl::TextRenderer renderer;
    renderer.load_font("arial", font_data(), 0);

    const std::string text = "the quick brown fox jumps over the lazy dog";
    const auto opts = base_options();
    const int unwrapped = single_line_width(renderer, text);

    opengl::Text t;
    auto wrap_opts = opts;
    wrap_opts.max_width = unwrapped / 2;
    wrap_opts.wrap = opengl::WrapMode::Word;
    renderer.render_text(t, text, fonts, wrap_opts);

    CHECK(t.size.x <= wrap_opts.max_width);
    CHECK(t.size.y > opts.size);
  }

  TEST_CASE("Wrap None ignores max_width")
  {
    const GlContext gl;
    if (skip_if_no_context_or_font(gl)) return;

    opengl::TextRenderer renderer;
    renderer.load_font("arial", font_data(), 0);

    const std::string text = "the quick brown fox";
    const auto opts = base_options();
    const int unwrapped = single_line_width(renderer, text);

    opengl::Text t;
    auto none_opts = opts;
    none_opts.max_width = 10;
    none_opts.wrap = opengl::WrapMode::None;
    renderer.render_text(t, text, fonts, none_opts);

    CHECK(t.size.x == unwrapped);
  }

  TEST_CASE("Char wrap breaks mid-word")
  {
    const GlContext gl;
    if (skip_if_no_context_or_font(gl)) return;

    opengl::TextRenderer renderer;
    renderer.load_font("arial", font_data(), 0);

    const std::string text = "abcdefghijklmnopqrstuvwxyz";
    const auto opts = base_options();
    const int unwrapped = single_line_width(renderer, text);

    opengl::Text t;
    auto char_opts = opts;
    char_opts.max_width = unwrapped / 4;
    char_opts.wrap = opengl::WrapMode::Char;
    renderer.render_text(t, text, fonts, char_opts);

    CHECK(t.size.x <= char_opts.max_width);
    CHECK(t.size.y > opts.size);
  }

  TEST_CASE("max_height clips the texture height")
  {
    const GlContext gl;
    if (skip_if_no_context_or_font(gl)) return;

    opengl::TextRenderer renderer;
    renderer.load_font("arial", font_data(), 0);

    const std::string text = "the quick brown fox jumps over the lazy dog";
    const auto opts = base_options();

    opengl::Text t;
    auto wrap_opts = opts;
    wrap_opts.max_width = single_line_width(renderer, text) / 4;
    wrap_opts.max_height = static_cast<std::int32_t>(opts.size);
    wrap_opts.wrap = opengl::WrapMode::Word;
    renderer.render_text(t, text, fonts, wrap_opts);

    CHECK(t.size.y <= wrap_opts.max_height);
  }

  TEST_CASE("custom line_height increases line spacing")
  {
    const GlContext gl;
    if (skip_if_no_context_or_font(gl)) return;

    opengl::TextRenderer renderer;
    renderer.load_font("arial", font_data(), 0);

    const std::string text = "the quick brown fox jumps over the lazy dog";
    const auto opts = base_options();
    const int half = single_line_width(renderer, text) / 2;

    opengl::Text t_default;
    auto opts_default = opts;
    opts_default.max_width = half;
    opts_default.wrap = opengl::WrapMode::Word;
    renderer.render_text(t_default, text, fonts, opts_default);

    opengl::Text t_tall;
    auto opts_tall = opts;
    opts_tall.max_width = half;
    opts_tall.wrap = opengl::WrapMode::Word;
    opts_tall.line_height = static_cast<float>(opts.size) * 2.0f;
    renderer.render_text(t_tall, text, fonts, opts_tall);

    CHECK(t_tall.size.y > t_default.size.y);
  }

  TEST_CASE("empty string produces zero-size texture")
  {
    const GlContext gl;
    if (skip_if_no_context_or_font(gl)) return;

    opengl::TextRenderer renderer;
    renderer.load_font("arial", font_data(), 0);

    opengl::Text t;
    renderer.render_text(t, "", fonts, base_options());

    CHECK(t.size.x == 0);
    CHECK(t.size.y == 0);
  }
}
