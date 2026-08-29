#pragma once

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

struct SpotShadowMap
{
  std::unique_ptr<::opengl::Texture> shadow_texture;
  std::unique_ptr<::opengl::Framebuffer> shadow_framebuffer;

  void setup(const GLsizei shadow_map_size)
  {
    shadow_texture = std::make_unique<::opengl::Texture>();
    shadow_texture->load_texture_array(
      0, GL_DEPTH_COMPONENT, shadow_map_size, shadow_map_size,
      static_cast<GLsizei>(kSpotShadowLimit), 0, GL_DEPTH_COMPONENT, GL_FLOAT,
      nullptr
    );
    shadow_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    shadow_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    shadow_texture->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    shadow_texture->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
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

  void render(
    SpotLightContext& light_context,
    const GLsizei shadow_map_size,
    const float shadow_far,
    const PipelineContext& pipeline_context
  )
  {
    const auto& light = *light_context.light;
    // Perspective from the spot light along its direction. Use 2x the outer
    // cone angle so the projection covers the full visible cone (the cone
    // half-angle is outer_cut_off degrees).
    const float fov
      = glm::radians(glm::clamp(light.outer_cut_off, 0.0f, 89.5f) * 2.0f);
    const glm::mat4 light_proj = glm::perspective(fov, 1.0f, 0.1f, shadow_far);
    const glm::vec3 forward = glm::normalize(light.direction);
    const glm::vec3 world_up(0.0f, 1.0f, 0.0f);
    const glm::vec3 up = glm::abs(glm::dot(forward, world_up)) > 0.99f
      ? glm::vec3(0.0f, 0.0f, 1.0f)
      : world_up;
    const glm::mat4 light_view
      = glm::lookAt(light.position, light.position + forward, up);
    light_context.light_pv = light_proj * light_view;

    glViewport(0, 0, shadow_map_size, shadow_map_size);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);

    AXGL_PROFILE_SCOPE("Render Spot Shadow Map");
    shadow_framebuffer->use();
    shadow_framebuffer->attach_texture_layer(
      GL_DEPTH_ATTACHMENT, *shadow_texture, light_context.shadow_index
    );
    shadow_framebuffer->check_status_complete();
    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);

    ShadowPassContext shadow_pass_context{
      .projection_view_matrix = light_context.light_pv
    };
    for (const auto& render_func : pipeline_context.shadow_pass)
      render_func(shadow_pass_context);
  }
};

} // namespace axgl::impl::opengl::renderer
