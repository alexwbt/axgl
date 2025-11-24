
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/component/camera.hpp>

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::RealmService>();

  // window
  const auto window = axgl.window_service()->create_window();
  window->set_title("Hello triangle!");

  // renderer
  const auto renderer_service = axgl.renderer_service();
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);

  // realm
  const auto realm_service = axgl.realm_service();
  const auto realm = realm_service->create_realm();
  realm->set_renderer(renderer);

  // triangle entity
  const auto entity = realm_service->create_entity<axgl::interface::Entity>();
  {
    // material
    const auto material = renderer_service->create_material("2d");
    material->set_color({ 1.0f, 0.5f, 0.2f, 1.0f });

    // triangle mesh
    const auto mesh_comp = realm_service->create_component<axgl::interface::component::Mesh>();
    mesh_comp->set_vertices(std::vector<glm::vec2>{ { 0.8f, -0.5f }, { -0.8f, -0.5f }, { 0.0f, 0.5f } });
    mesh_comp->set_material(material);
    entity->add_component(mesh_comp);

    // camera
    const auto camera_comp = realm_service->create_component<axgl::impl::component::Camera>();
    camera_comp->camera.orthographic = true;
    camera_comp->camera.near_clip = -1;
    camera_comp->camera.far_clip = 1;
    entity->add_component(camera_comp);
  }
  entity->transform()->scale = glm::vec3(200.0f);
  entity->update_model_matrix();
  realm->add_entity(entity);

  // start
  axgl.run();
}
