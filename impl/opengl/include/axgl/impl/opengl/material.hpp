#pragma once

#include <axgl/interface/material.hpp>

#include <axgl/impl/opengl/renderer/render_component.hpp>

#include <opengl/static_shaders.hpp>

namespace axgl::impl::opengl
{

class Material : public axgl::Material
{
public:
  enum Attribute
  {
    kVertices,
    kNormals,
    kUV,
    kModels,
  };

protected:
  glm::vec4 color_{1.0f, 1.0f, 1.0f, 1.0f};
  float line_width_ = 1.0f;
  axgl::Material::DrawMode draw_mode_ = axgl::Material::DrawMode::kTriangles;
  axgl::Material::CullMode cull_mode_ = axgl::Material::CullMode::kCCW;
  bool enable_depth_test_ = true;
  bool enable_blend_ = false;
  float alpha_discard_ = 0.0f;
  glm::vec2 uv_scale_{1.0f};
  glm::vec2 uv_offset_{1.0f};

public:
  void set_color(const glm::vec4& color) override { color_ = color; }
  void set_line_width(float line_width) override { line_width_ = line_width; }
  void set_draw_mode(axgl::Material::DrawMode draw_mode) override { draw_mode_ = draw_mode; }
  void set_cull_mode(const axgl::Material::CullMode cull_mode) override { cull_mode_ = cull_mode; }
  void set_enable_depth_test(const bool enable_depth_test) override { enable_depth_test_ = enable_depth_test; }
  void set_enable_blend(const bool enable_blend) override { enable_blend_ = enable_blend; }
  void set_alpha_discard(const float alpha_discard) override { alpha_discard_ = alpha_discard; }
  void set_tiling(glm::vec2 tiling) override { uv_scale_ = tiling; }
  void set_offset(glm::vec2 offset) override { uv_offset_ = offset; }

  void set_property(const std::string& key, const std::string& value) override
  {
    AXGL_LOG_DEBUG("Properties are not supported in color material.");
  }

  void add_texture(const axgl::Material::TextureType type, const axgl::ptr_t<axgl::Texture> texture) override
  {
    AXGL_LOG_DEBUG("Textures are not supported in color material.");
  }

  [[nodiscard]] bool enabled_blend() const { return enable_blend_; }
  [[nodiscard]] GLenum draw_mode() const
  {
    switch (draw_mode_)
    {
    case DrawMode::kPoints: return GL_POINTS;
    case DrawMode::kLines: return GL_LINES;
    case DrawMode::kLineStrip: return GL_LINE_STRIP;
    case DrawMode::kLineLoop: return GL_LINE_LOOP;
    default:
    case DrawMode::kTriangles: return GL_TRIANGLES;
    case DrawMode::kTriangleStrip: return GL_TRIANGLE_STRIP;
    case DrawMode::kTriangleFan: return GL_TRIANGLE_FAN;
    }
  }

  virtual void use(const impl::opengl::renderer::RenderContext& context)
  {
    if (enable_depth_test_)
    {
      glDepthFunc(GL_LESS);
      glDepthRange(0.0f, 1.0f);
    }
    else
    {
      glDepthFunc(GL_ALWAYS);
      glDepthRange(0.0f, 0.0f);
    }

    glLineWidth(line_width_);
    glCullFace(GL_FRONT);
    switch (cull_mode_)
    {
    case axgl::Material::CullMode::kCW:
      glEnable(GL_CULL_FACE);
      glFrontFace(GL_CW);
      break;
    case axgl::Material::CullMode::kCCW:
      glEnable(GL_CULL_FACE);
      glFrontFace(GL_CCW);
      break;
    case axgl::Material::CullMode::kNone: glDisable(GL_CULL_FACE); break;
    }
  }

  [[nodiscard]] virtual int get_attribute_offset(const Attribute attribute) const
  {
    switch (attribute)
    {
    case kVertices: return 0;
    case kNormals: return 1;
    case kUV: return 2;
    case kModels: return 3;
    default: return 4;
    }
  }
};

} // namespace axgl::impl::opengl

