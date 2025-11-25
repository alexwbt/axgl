
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::GlfwInputService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::RealmService>();
  axgl.use_service<axgl::impl::EntityService>();
  axgl.use_service<axgl::impl::CameraService>();
  axgl.initialize();

  // window
  const auto window = axgl.window_service()->create_window();
  window->set_title("Hello triangle!");

  // renderer
  const auto renderer_service = axgl.renderer_service();
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);

  const auto entity_service = axgl.entity_service();
  // realm
  const auto realm = axgl.realm_service()->create_realm();
  realm->set_renderer(renderer);

  // triangle entity
  const auto entity = entity_service->create_entity();
  {
    // material
    const auto material = renderer_service->create_material("2d");
    material->set_color({1.0f, 0.5f, 0.2f, 1.0f});

    // triangle mesh
    const auto mesh_comp = entity_service->create_component_t<axgl::impl::component::OpenglMesh>();
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
  entity->transform()->scale = glm::vec3(200.0f);
  entity->update_model_matrix();
  realm->add_entity(entity);

  // start
  axgl.run();
  axgl.terminate();
}
