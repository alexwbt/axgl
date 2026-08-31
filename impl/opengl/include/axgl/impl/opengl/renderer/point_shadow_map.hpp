#pragma once

#include <array>
#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <axgl/impl/opengl/renderer/constants.hpp>
#include <axgl/impl/opengl/renderer/render_context.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct PointShadowMap
{
  std::unique_ptr<::opengl::Texture> shadow_texture;
  std::unique_ptr<::opengl::Framebuffer> shadow_framebuffer;

  void setup(const GLsizei shadow_map_size)
  {
    shadow_texture = std::make_unique<::opengl::Texture>();
    shadow_texture->load_cubemap_array(
      0, GL_DEPTH_COMPONENT, shadow_map_size, shadow_map_size,
      static_cast<GLsizei>(kPointShadowLimit), 0, GL_DEPTH_COMPONENT, GL_FLOAT,
      nullptr
    );
    shadow_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    shadow_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    shadow_texture->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    shadow_texture->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    shadow_texture->set_parameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    shadow_texture->set_parameter(
      GL_TEXTURE_BORDER_COLOR, std::array{1.0f, 1.0f, 1.0f, 1.0f}
    );
    shadow_framebuffer = std::make_unique<::opengl::Framebuffer>();
  }

  void reset()
  {
    shadow_framebuffer.reset();
    shadow_texture.reset();
  }

  static std::array<glm::mat4, 6> get_face_views(
    const glm::vec3& position, const float shadow_far
  )
  {
    const glm::mat4 proj
      = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, shadow_far);
    return {
      proj
        * glm::lookAt(
          position, position + glm::vec3(1.0f, 0.0f, 0.0f),
          glm::vec3(0.0f, -1.0f, 0.0f)
        ),
      proj
        * glm::lookAt(
          position, position + glm::vec3(-1.0f, 0.0f, 0.0f),
          glm::vec3(0.0f, -1.0f, 0.0f)
        ),
      proj
        * glm::lookAt(
          position, position + glm::vec3(0.0f, 1.0f, 0.0f),
          glm::vec3(0.0f, 0.0f, 1.0f)
        ),
      proj
        * glm::lookAt(
          position, position + glm::vec3(0.0f, -1.0f, 0.0f),
          glm::vec3(0.0f, 0.0f, -1.0f)
        ),
      proj
        * glm::lookAt(
          position, position + glm::vec3(0.0f, 0.0f, 1.0f),
          glm::vec3(0.0f, -1.0f, 0.0f)
        ),
      proj
        * glm::lookAt(
          position, position + glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec3(0.0f, -1.0f, 0.0f)
        )
    };
  }

  void render(
    PointLightContext& light_context,
    const GLsizei shadow_map_size,
    const float shadow_far,
    const PipelineContext& pipeline_context
  )
  {
    const auto& light = *light_context.light;
    light_context.far_plane = shadow_far;

    const auto face_views = get_face_views(light.position, shadow_far);

    glViewport(0, 0, shadow_map_size, shadow_map_size);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);

    AXGL_PROFILE_SCOPE("Render Point Shadow Map");
    for (GLsizei face = 0; face < 6; ++face)
    {
      shadow_framebuffer->use();
      shadow_framebuffer->attach_cubemap_face(
        GL_DEPTH_ATTACHMENT, *shadow_texture, light_context.shadow_index, face
      );
      shadow_framebuffer->check_status_complete();
      glClearDepth(1.0);
      glClear(GL_DEPTH_BUFFER_BIT);

      ShadowPassContext shadow_pass_context{
        .light_type = axgl::Light::Type::kPoint,
        .projection_view_matrix = face_views[face],
        .light_position = light.position,
        .far_plane = shadow_far
      };
      for (const auto& render_func : pipeline_context.shadow_pass)
        render_func(shadow_pass_context);
    }
  }
};

} // namespace axgl::impl::opengl::renderer
