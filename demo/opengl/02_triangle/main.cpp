
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

int main()
{
  axgl::Axgl axgl;
  axgl::configure_default(axgl);
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
  axgl.initialize();

  const auto realm_service = axgl.realm_service();
  const auto window_service = axgl.window_service();
  const auto renderer_service = axgl.renderer_service();
  const auto entity_service = axgl.entity_service();
  const auto camera_service = axgl.camera_service();

  // window
  const auto window = window_service->create_window();
  window->set_title("Hello triangle!");

  // renderer
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);

  // realm
  const auto realm = realm_service->create_realm();
  realm_service->set_active_realm(realm);
  realm->set_renderer(renderer);

  // triangle entity
  const auto entity = entity_service->create_entity();
  {
    // material
    const auto material = renderer_service->create_material("2d");
    material->set_color({1.0f, 0.5f, 0.2f, 1.0f});

    // triangle mesh
    const auto mesh_comp = entity_service->create_component_t<axgl::impl::opengl::component::Mesh>();
    mesh_comp->set_vertices(std::vector<glm::vec2>{{0.8f, -0.5f}, {-0.8f, -0.5f}, {0.0f, 0.5f}});
    mesh_comp->set_material(material);
    entity->components()->add(mesh_comp);

    // camera
    const auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
    camera_comp->camera.orthographic = true;
    camera_comp->camera.near_clip = -1;
    camera_comp->camera.far_clip = 1;
    entity->components()->add(camera_comp);
  }
  entity->transform().scale = glm::vec3(200.0f);
  entity->update_model_matrix();
  realm->add_entity(entity);

  // set camera
  camera_service->set_camera(entity);

  // start
  axgl.run();
  axgl.terminate();
}
