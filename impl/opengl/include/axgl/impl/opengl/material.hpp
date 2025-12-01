#pragma once

#include <memory>

#include <axgl/interface/components/mesh.hpp>

#include <opengl/static_shaders.hpp>

namespace axgl::impl::opengl
{

class Material : public axgl::Material
{
protected:
  glm::vec4 color_{1.0f, 1.0f, 1.0f, 1.0f};
  axgl::Material::CullMode cull_mode_ = axgl::Material::CullMode::kCCW;
  bool enable_blend_ = false;

public:
  void set_color(const glm::vec4& color) override { color_ = color; }
  void set_cull_mode(const axgl::Material::CullMode cull_mode) override { cull_mode_ = cull_mode; }
  void set_enable_blend(const bool enable_blend) override { enable_blend_ = enable_blend; }

  [[nodiscard]] bool enabled_blend() const { return enable_blend_; }

  virtual void use(const axgl::Camera* camera, const axgl::Entity* entity, const axgl::component::Mesh& mesh)
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

    if (enable_blend_)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
      glDisable(GL_BLEND);
  }
};

} // namespace axgl::impl::opengl

