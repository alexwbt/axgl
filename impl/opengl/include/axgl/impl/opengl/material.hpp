#pragma once

#include <axgl/interface/material.hpp>

#include <axgl/impl/opengl/render_component.hpp>

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
  float gloss_ = 0.0625f;
  glm::vec4 color_{1.0f, 1.0f, 1.0f, 1.0f};
  axgl::Material::CullMode cull_mode_ = axgl::Material::CullMode::kCCW;
  bool enable_blend_ = false;
  float alpha_discard_ = 0.0f;

public:
  void set_gloss(const float gloss) override { gloss_ = gloss; }
  void set_color(const glm::vec4& color) override { color_ = color; }
  void set_cull_mode(const axgl::Material::CullMode cull_mode) override { cull_mode_ = cull_mode; }
  void set_enable_blend(const bool enable_blend) override { enable_blend_ = enable_blend; }
  void set_alpha_discard(const float alpha_discard) override { alpha_discard_ = alpha_discard; }

  [[nodiscard]] bool enabled_blend() const { return enable_blend_; }

  virtual void use(const RenderComponent::Context& context)
  {
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

  [[nodiscard]] virtual int get_attribute_offset(Attribute attribute) const = 0;
};

} // namespace axgl::impl::opengl

