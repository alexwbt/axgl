#pragma once

#include <vector>

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/component/mesh.hpp>

#include "opengl/shader_program.hpp"
#include "opengl/vertex_array_object.hpp"

#include "axgl_opengl_impl/res.hpp"

NAMESPACE_AXGL_IMPL

class OpenglMesh3D : public interface::Mesh3D
{
private:
  opengl::ShaderProgram shader_{ {
    { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.vs") },
    { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.fs") }
  } };
  opengl::VertexArrayObject vertex_array_;
  glm::vec3 color_{ 1.0f, 1.0f, 1.0f };
  float specular_ = 1.0f;

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
        shader_.set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].direction", light.direction);
        shader_.set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].ambient", light.color.ambient);
        shader_.set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].diffuse", light.color.diffuse);
        shader_.set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].specular", light.color.specular);
        sun_lights_size++;
        break;
      case (interface::Light::POINT):
        shader_.set_vec3("point_lights[" + std::to_string(point_lights_size) + "].position", light.position);
        shader_.set_vec3("point_lights[" + std::to_string(point_lights_size) + "].ambient", light.color.ambient);
        shader_.set_vec3("point_lights[" + std::to_string(point_lights_size) + "].diffuse", light.color.diffuse);
        shader_.set_vec3("point_lights[" + std::to_string(point_lights_size) + "].specular", light.color.specular);
        shader_.set_float("point_lights[" + std::to_string(point_lights_size) + "].constant", light.strength.constant);
        shader_.set_float("point_lights[" + std::to_string(point_lights_size) + "].linear", light.strength.linear);
        shader_.set_float("point_lights[" + std::to_string(point_lights_size) + "].quadratic", light.strength.quadratic);
        point_lights_size++;
        break;
      case (interface::Light::SPOT):
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].direction", light.direction);
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].position", light.position);
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].ambient", light.color.ambient);
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].diffuse", light.color.diffuse);
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].specular", light.color.specular);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].constant", light.strength.constant);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].linear", light.strength.linear);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].quadratic", light.strength.quadratic);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].cutOff", light.cut_off);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].outerCutOff", light.outer_cut_off);
        spot_lights_size++;
        break;
      }
    }
    shader_.set_int("sun_lights_size", sun_lights_size);
    shader_.set_int("spot_lights_size", spot_lights_size);
    shader_.set_int("point_lights_size", point_lights_size);
  }

public:
  void render() override
  {
    auto context = get_context();
    glm::mat4 m = model();
    glm::mat4 mvp = context->pv * m;

    shader_.use_program();
    shader_.set_mat4("mvp", mvp);
    shader_.set_mat4("model", m);
    shader_.set_vec3("mesh_color", color_);
    shader_.set_float("mesh_specular", specular_);
    shader_.set_vec3("camera_pos", context->realm->camera.position);

    use_lights(context->realm->lights);

    vertex_array_.draw();
  }

  void set_vertices(const std::span<const glm::vec3>& vertices) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec3>(vertices, attributes, 0);
  }

  void set_normals(const std::span<const glm::vec3>& normals) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec3>(normals, attributes, 1);
  }

  void set_uv(const std::span<const glm::vec2>& uv) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec2), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec2>(uv, attributes, 2);
  }

  void set_indices(const std::span<const uint32_t>& indices) override
  {
    vertex_array_.create_element_buffer(indices);
  }

  void set_color(const glm::vec3& color) override
  {
    color_ = color;
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<interface::Mesh3D> Axgl::create_component()
{
  return std::make_shared<impl::OpenglMesh3D>();
}

NAMESPACE_AXGL_END
