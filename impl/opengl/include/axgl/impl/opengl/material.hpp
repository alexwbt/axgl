#pragma once

#include <string>
#include <memory>

#include <spdlog/spdlog.h>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/component/mesh.hpp>
#include <axgl/impl/opengl/texture.hpp>

#include <opengl/static_shaders.hpp>

NAMESPACE_AXGL_IMPL

class OpenglMaterial : public interface::Material
{
public:
  virtual void use(
    const interface::RealmContext* context,
    const interface::Mesh* mesh
  ) = 0;
};

class OpenglDefaultMaterial : public OpenglMaterial
{
private:
  std::shared_ptr<opengl::ShaderProgram> shader_ = opengl::StaticShaders::instance().mesh_3d();
  glm::vec3 color_{ 1.0f, 1.0f, 1.0f };
  float shininess_ = 1.0f;

  std::shared_ptr<OpenglTexture> diffuse_texture_;
  std::shared_ptr<OpenglTexture> specular_texture_;
  std::shared_ptr<OpenglTexture> normal_texture_;
  std::shared_ptr<OpenglTexture> height_texture_;

public:
  void set_color(const glm::vec3& color) override
  {
    color_ = color;
  }

  void set_prop(const std::string& key, const std::string& value) override
  {
    if (key == "shininess")
      shininess_ = std::stof(value);
#ifdef AXGL_DEBUG
    else
      throw std::runtime_error("Invalid material prop key: " + key);
#endif
  }

  void add_texture(interface::TextureType type, std::shared_ptr<interface::Texture> texture) override
  {
    auto texture_ = std::dynamic_pointer_cast<OpenglTexture>(texture);
    if (!texture_)
#ifdef AXGL_DEBUG
      throw std::runtime_error("The provided texture is not a valid opengl texture.");
#else
      return;
#endif
    using enum interface::TextureType;
    switch (type)
    {
    case DIFFUSE: diffuse_texture_ = std::move(texture_); break;
    case SPECULAR: specular_texture_ = std::move(texture_); break;
    case NORMAL: normal_texture_ = std::move(texture_); break;
    case HEIGHT: height_texture_ = std::move(texture_); break;
#ifdef AXGL_DEBUG
    case UNKNOWN: SPDLOG_WARN("Adding texture with UNKNOWN type."); break;
#endif
    }
  }

  void use(const interface::RealmContext* context, const interface::Mesh* mesh) override
  {
    glm::mat4 model = mesh->get_model();
    glm::mat4 mvp = context->pv * model;

    shader_->use_program();
    shader_->set_mat4("mvp", mvp);
    shader_->set_mat4("model", model);
    shader_->set_vec3("camera_pos", context->realm->camera.position);
    shader_->set_vec3("mesh_color", color_);
    shader_->set_float("mesh_shininess", shininess_);

    use_lights(context->realm->lights);

    use_texture(0, "diffuse", diffuse_texture_);
    use_texture(1, "specular", specular_texture_);
    use_texture(2, "normal", normal_texture_);
    use_texture(3, "height", height_texture_);
  }

private:
  void use_lights(const std::vector<interface::Light>& lights)
  {
    int sun_lights_size = 0;
    int spot_lights_size = 0;
    int point_lights_size = 0;

    for (const auto& light : lights)
    {
      switch (light.type)
      {
      case (interface::Light::SUN):
        shader_->set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].direction", light.direction);
        shader_->set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].ambient", light.color.ambient);
        shader_->set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].diffuse", light.color.diffuse);
        shader_->set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].specular", light.color.specular);
        sun_lights_size++;
        break;
      case (interface::Light::POINT):
        shader_->set_vec3("point_lights[" + std::to_string(point_lights_size) + "].position", light.position);
        shader_->set_vec3("point_lights[" + std::to_string(point_lights_size) + "].ambient", light.color.ambient);
        shader_->set_vec3("point_lights[" + std::to_string(point_lights_size) + "].diffuse", light.color.diffuse);
        shader_->set_vec3("point_lights[" + std::to_string(point_lights_size) + "].specular", light.color.specular);
        shader_->set_float("point_lights[" + std::to_string(point_lights_size) + "].constant", light.strength.constant);
        shader_->set_float("point_lights[" + std::to_string(point_lights_size) + "].linear", light.strength.linear);
        shader_->set_float("point_lights[" + std::to_string(point_lights_size) + "].quadratic", light.strength.quadratic);
        point_lights_size++;
        break;
      case (interface::Light::SPOT):
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].direction", light.direction);
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].position", light.position);
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].ambient", light.color.ambient);
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].diffuse", light.color.diffuse);
        shader_->set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].specular", light.color.specular);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].constant", light.strength.constant);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].linear", light.strength.linear);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].quadratic", light.strength.quadratic);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].cutOff", light.cut_off);
        shader_->set_float("spot_lights[" + std::to_string(spot_lights_size) + "].outerCutOff", light.outer_cut_off);
        spot_lights_size++;
        break;
      }
    }
    shader_->set_int("sun_lights_size", sun_lights_size);
    shader_->set_int("spot_lights_size", spot_lights_size);
    shader_->set_int("point_lights_size", point_lights_size);
  }

  void use_texture(
    int i,
    const std::string& name,
    const std::shared_ptr<OpenglTexture>& texture)
  {
    if (!texture) return;

    glActiveTexture(GL_TEXTURE0 + i);
    texture->use();
    shader_->set_int(name + "_texture", i);
    shader_->set_bool("use_" + name + "_texture", true);
  }
};

class OpenglDefault2DMaterial : public OpenglMaterial
{
private:
  std::shared_ptr<opengl::ShaderProgram> shader_ = opengl::StaticShaders::instance().mesh_2d();
  glm::vec3 color_{ 1.0f, 1.0f, 1.0f };
  std::shared_ptr<OpenglTexture> texture_;

public:
  void set_color(const glm::vec3& color) override
  {
    color_ = color;
  }

  void set_prop(const std::string& key, const std::string& value) override {}

  void add_texture(interface::TextureType type, std::shared_ptr<interface::Texture> texture) override
  {
    texture_ = std::dynamic_pointer_cast<OpenglTexture>(texture);
#ifdef AXGL_DEBUG
    if (!texture_)
      throw std::runtime_error("The provided texture is not a valid opengl texture.");
#endif
  }

  void use(const interface::RealmContext* context, const interface::Mesh* mesh) override
  {
    glm::mat4 model = mesh->get_model();
    glm::mat4 mvp = context->pv * model;

    shader_->use_program();
    shader_->set_mat4("mvp", mvp);
    shader_->set_vec3("mesh_color", color_);

    if (texture_)
    {
      glActiveTexture(GL_TEXTURE0);
      texture_->use();
      shader_->set_int("mesh_texture", 0);
      shader_->set_bool("use_texture", true);
    }
  }
};

NAMESPACE_AXGL_IMPL_END
