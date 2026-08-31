#pragma once

#include <axgl/interface/material.hpp>

#include <axgl/impl/opengl/renderer/render_component.hpp>

#include <opengl/shader_program.hpp>

namespace axgl::impl::opengl
{

class Material : public axgl::Material
{
public:
  enum Attribute
  {
    kVertices,
    kNormals,
    kTangents,
    kBitangents,
    kUV,
    kModels,
  };

protected:
  glm::vec4 color_{1.0f, 1.0f, 1.0f, 1.0f};
  bool enable_depth_test_ = true;
  bool enable_blend_ = false;
  float alpha_discard_ = 0.0f;
  float height_scale_ = 0.1f;
  float normal_scale_ = 1.0f;
  glm::vec2 uv_scale_{1.0f};
  glm::vec2 uv_offset_{1.0f};

public:
  void set_color(const glm::vec4& color) override { color_ = color; }
  void set_enable_depth_test(const bool enable_depth_test) override
  {
    enable_depth_test_ = enable_depth_test;
  }
  void set_enable_blend(const bool enable_blend) override
  {
    enable_blend_ = enable_blend;
  }
  void set_alpha_discard(const float alpha_discard) override
  {
    alpha_discard_ = alpha_discard;
  }
  void set_tiling(glm::vec2 tiling) override { uv_scale_ = tiling; }
  void set_offset(glm::vec2 offset) override { uv_offset_ = offset; }
  void set_depth_scale(float depth_scale) override
  {
    height_scale_ = depth_scale;
  }
  void set_normal_scale(float normal_scale) override
  {
    normal_scale_ = normal_scale;
  }
  void set_property(const std::string&, const std::string&) override
  {
    AXGL_LOG_DEBUG("Properties are not supported.");
  }
  void add_texture(
    const axgl::Material::TextureType, const axgl::ptr_t<axgl::Texture>
  ) override
  {
    AXGL_LOG_DEBUG("Textures are not supported.");
  }
  [[nodiscard]] glm::vec4 get_color() const override { return color_; }
  [[nodiscard]] bool get_enable_depth_test() const override
  {
    return enable_depth_test_;
  }
  [[nodiscard]] bool get_enable_blend() const override { return enable_blend_; }
  [[nodiscard]] float get_alpha_discard() const override
  {
    return alpha_discard_;
  }
  [[nodiscard]] glm::vec2 get_tiling() const override { return uv_scale_; }
  [[nodiscard]] glm::vec2 get_offset() const override { return uv_offset_; }
  [[nodiscard]] float get_depth_scale() const override { return height_scale_; }
  [[nodiscard]] float get_normal_scale() const override
  {
    return normal_scale_;
  }
  [[nodiscard]] std::string get_property(const std::string&) const override
  {
    return {};
  }
  [[nodiscard]] axgl::ptr_t<axgl::Texture> get_texture(
    axgl::Material::TextureType
  ) const override
  {
    return nullptr;
  }

  virtual void use(const renderer::RenderContext&)
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
  }

  [[nodiscard]] virtual int get_attribute_offset(
    const Attribute attribute
  ) const
  {
    switch (attribute)
    {
    case kVertices: return 0;
    case kNormals: return 1;
    case kTangents: return 2;
    case kBitangents: return 3;
    case kUV: return 4;
    case kModels: return 5;
    default: return 6;
    }
  }

  [[nodiscard]] virtual const ::opengl::ShaderProgram* get_shader() const
  {
    return nullptr;
  }
};

} // namespace axgl::impl::opengl
