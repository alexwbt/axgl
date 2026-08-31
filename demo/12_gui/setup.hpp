#pragma once

#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif
#include <axgl/common/color.hpp>
#include <axgl/common/mesh.hpp>
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>

inline void setup(axgl::Axgl& axgl)
{
  axgl::configure_default(axgl);
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
#endif

  axgl.initialize();

  const auto& window_service = axgl.window_service();
  const auto& input_service = axgl.input_service();
  const auto& renderer_service = axgl.renderer_service();
  const auto& realm_service = axgl.realm_service();
  const auto& entity_service = axgl.entity_service();
  const auto& camera_service = axgl.camera_service();

  // window
  const auto window = window_service->create_window();
  window->set_title("Hello gui!");
  window->set_size(1000, 800);

  // input
  input_service->set_window(window);

  // renderer
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);
  renderer->set_enable_msaa(true);
  renderer->set_msaa_sample_count(8);
  renderer_service->set_active_renderer(renderer);

  // realm
  const auto realm = realm_service->create_realm();
  realm_service->set_active_realm(realm);

  // camera entity
  {
    const auto camera_entity = entity_service->create_entity();
    const auto camera_comp
      = entity_service->create_component_t<axgl::impl::component::Camera>();
    camera_entity->add_component(camera_comp);
    realm->entities().add(camera_entity);
    camera_entity->transform().position.z = -2;
    camera_entity->transform().update_matrix();
    camera_service->set_camera_entity(camera_entity);

    // camera input
    camera_service->set_camera_mode(
      axgl::create_ptr<axgl::impl::camera::Keyboard3DFreeFlyCameraMode>()
    );
    camera_service->set_camera_entity(camera_entity);
  }

  // light entity
  {
    const auto light_entity = entity_service->create_entity();
    const auto light_comp
      = entity_service->create_component_t<axgl::impl::component::Light>();
    light_comp->light = axgl::Light::sunlight({0.2f, -1.0f, 1.2f});
    light_entity->add_component(light_comp);
    realm->entities().add(light_entity);
  }

  // cube entity
  {
    // material
    const auto material = renderer_service->create_material("3d");
    material->set_color(axgl::colors::kSkyBlue);

    // cube mesh
    const auto mesh_comp
      = entity_service->create_component_t<axgl::component::Mesh>();
    axgl::mesh::init_cube(*mesh_comp);
    mesh_comp->set_material(material);

    const auto cube_entity = entity_service->create_entity();
    cube_entity->add_component(mesh_comp);
    realm->entities().add(cube_entity);
  }
}
