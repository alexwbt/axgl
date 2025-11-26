#pragma once

#include <memory>
#include <span>
#include <string>

#include <spdlog/spdlog.h>

#include <axgl/interface/components/mesh.hpp>

#include <axgl/impl/opengl/texture.hpp>

#include <opengl/static_shaders.hpp>

namespace axgl::impl
{

class OpenglMaterial : public axgl::Material
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

  virtual void use(const axgl::Realm::Context* context, const axgl::component::Mesh* mesh)
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

class OpenglDefaultMaterial : public OpenglMaterial
{
  std::shared_ptr<opengl::ShaderProgram> shader_ = opengl::StaticShaders::instance().mesh_3d();
  float shininess_ = 1.0f;

  std::shared_ptr<OpenglTexture> diffuse_texture_;
  std::shared_ptr<OpenglTexture> specular_texture_;
  std::shared_ptr<OpenglTexture> normal_texture_;
  std::shared_ptr<OpenglTexture> height_texture_;

public:
  void set_prop(const std::string& key, const std::string& value) override
  {
    if (key == "shininess")
      shininess_ = std::stof(value);
#ifdef AXGL_DEBUG
    else
      throw std::runtime_error("Invalid material prop key: " + key);
#endif
  }

  void add_texture(axgl::Material::TextureType type, std::shared_ptr<axgl::Texture> texture) override
  {
    auto texture_ = std::dynamic_pointer_cast<OpenglTexture>(texture);
    if (!texture_)
#ifdef AXGL_DEBUG
      throw std::runtime_error("The provided texture is not a valid opengl texture.");
#else
      return;
#endif
    using enum axgl::Material::TextureType;
    switch (type)
    {
    case kDiffuse: diffuse_texture_ = std::move(texture_); break;
    case kSpecular: specular_texture_ = std::move(texture_); break;
    case kNormal: normal_texture_ = std::move(texture_); break;
    case kHeight: height_texture_ = std::move(texture_); break;
#ifdef AXGL_DEBUG
    case kUnknown: SPDLOG_WARN("Adding texture with UNKNOWN type."); break;
#endif
    }
  }

  void use(const axgl::Realm::Context* context, const axgl::component::Mesh* mesh) override
  {
    OpenglMaterial::use(context, mesh);

    const auto camera = context->axgl->camera_service()->get_camera();
    const auto lights = context->axgl->light_service()->get_lights();
    const auto model = mesh->get_entity()->get_model_matrix();
    const auto mvp = camera->projection_view_matrix() * model;

    shader_->use_program();
    shader_->set_mat4("mvp", mvp);
    shader_->set_mat4("model", model);
    shader_->set_vec3("camera_pos", camera->position);
    shader_->set_vec4("mesh_color", color_);
    shader_->set_float("mesh_shininess", shininess_);

    use_lights(lights);

    use_texture(0, "diffuse", diffuse_texture_);
    use_texture(1, "specular", specular_texture_);
    use_texture(2, "normal", normal_texture_);
    use_texture(3, "height", height_texture_);
  }

private:
  void use_lights(const std::span<const axgl::Light*>& lights) const
  {
    int sun_lights_size = 0;
    int spot_lights_size = 0;
    int point_lights_size = 0;

    for (const auto& light : lights)
    {
      switch (light->type)
      {
      case (axgl::Light::kSun):
        shader_->set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].direction", light->direction);
        shader_->set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].ambient", light->color.ambient);
        shader_->set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].diffuse", light->color.diffuse);
        shader_->set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].specular", light->color.specular);
        sun_lights_size++;
        break;
      case (axgl::Light::kPoint):
        shader_->set_vec3("point_lights[" + std::to_string(point_lights_size) + "].position", light->position);
        shader_->set_vec3("point_lights[" + std::to_string(point_lights_size) + "].ambient", light->color.ambient);
        shader_->set_vec3("point_lights[" + std::to_string(point_lights_size) + "].diffuse", light->color.diffuse);
        shader_->set_vec3("point_lights[" + std::to_string(point_lights_size) + "].specular", light->color.specular);
        shader_->set_float(
          "point_lights[" + std::to_string(point_lights_size) + "].constant", light->strength.constant);
        shader_->set_float("point_lights[" + std::to_string(point_lights_size) + "].linear", light->strength.linear);
        shader_->set_float(
          "point_lights[" + std::to_string(point_lights_size) + "].quadratic", light->strength.quadratic);
        point_lights_size++;
        break;
      case (axgl::Light::kSpot):
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].direction", light->direction);
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].position", light->position);
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].ambient", light->color.ambient);
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].diffuse", light->color.diffuse);
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].specular", light->color.specular);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].constant", light->strength.constant);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].linear", light->strength.linear);
        shader_->set_float(
          "spot_lights[" + std::to_string(spot_lights_size) + "].quadratic", light->strength.quadratic);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].cutOff", light->cut_off);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].outerCutOff", light->outer_cut_off);
        spot_lights_size++;
        break;
      }
    }
    shader_->set_int("sun_lights_size", sun_lights_size);
    shader_->set_int("spot_lights_size", spot_lights_size);
    shader_->set_int("point_lights_size", point_lights_size);
  }

  void use_texture(const int i, const std::string& name, const std::shared_ptr<OpenglTexture>& texture) const
  {
    if (!texture)
      return;

    glActiveTexture(GL_TEXTURE0 + i);
    texture->use();
    shader_->set_int(name + "_texture", i);
    shader_->set_bool("use_" + name + "_texture", true);
  }
};

class OpenglDefault2DMaterial : public OpenglMaterial
{
  std::shared_ptr<opengl::ShaderProgram> shader_ = opengl::StaticShaders::instance().mesh_2d();
  std::shared_ptr<OpenglTexture> texture_;

public:
  void set_prop(const std::string& key, const std::string& value) override { }

  void add_texture(axgl::Material::TextureType type, std::shared_ptr<axgl::Texture> texture) override
  {
    texture_ = std::dynamic_pointer_cast<OpenglTexture>(texture);
#ifdef AXGL_DEBUG
    if (!texture_)
      throw std::runtime_error("The provided texture is not a valid opengl texture.");
#endif
  }

  void use(const axgl::Realm::Context* context, const axgl::component::Mesh* mesh) override
  {
    OpenglMaterial::use(context, mesh);

    const auto camera = context->axgl->camera_service()->get_camera();
    const auto model = mesh->get_entity()->get_model_matrix();
    const auto mvp = camera->projection_view_matrix() * model;

    shader_->use_program();
    shader_->set_mat4("mvp", mvp);
    shader_->set_vec4("mesh_color", color_);

    if (texture_)
    {
      glActiveTexture(GL_TEXTURE0);
      texture_->use();
      shader_->set_int("mesh_texture", 0);
      shader_->set_bool("use_texture", true);
    }
    else
    {
      shader_->set_bool("use_texture", false);
    }
  }
};

} // namespace axgl::impl

