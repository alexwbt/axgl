#include <vector>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

#include <demo_opengl_texture/res.hpp>

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::GlfwInputService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::RealmService>();
  axgl.use_service<axgl::impl::EntityService>();
  axgl.use_service<axgl::impl::CameraService>();
  axgl.use_service<axgl::impl::LightService>();
  axgl.initialize();

  const auto window_service = axgl.window_service();
  const auto renderer_service = axgl.renderer_service();
  const auto realm_service = axgl.realm_service();
  const auto entity_service = axgl.entity_service();
  const auto camera_service = axgl.camera_service();

  // window
  const auto window = window_service->create_window();
  window->set_title("Hello texture!");

  // renderer
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);

  // realm
  const auto realm = realm_service->create_realm();
  realm->set_renderer(renderer);

  // square entity
  const auto square_entity = entity_service->create_entity();
  {
    // texture
    const auto texture = renderer_service->create_texture();
    texture->load_texture(demo_opengl_texture_res::get("container.png"));

    // material
    const auto material = renderer_service->create_material("2d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, texture);

    // square mesh
    const auto mesh_comp = entity_service->create_component_t<axgl::component::Mesh>();
    mesh_comp->set_vertices(
      std::vector<glm::vec2>{
        {0.5f, 0.5f},
        {0.5f, -0.5f},
        {-0.5f, -0.5f},
        {-0.5f, 0.5f},
      });
    mesh_comp->set_uv(
      std::vector<glm::vec2>{
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f},
      });
    mesh_comp->set_indices(std::vector<uint32_t>{0, 1, 2, 0, 2, 3});
    mesh_comp->set_material(material);
    square_entity->components()->add(mesh_comp);

    // camera
    const auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
    camera_comp->camera.orthographic = true;
    camera_comp->camera.near_clip = -1;
    camera_comp->camera.far_clip = 1;
    square_entity->components()->add(camera_comp);
  }
  square_entity->transform()->scale = glm::vec3(200.0f);
  square_entity->update_model_matrix();
  realm->add_entity(square_entity);

  // set camera
  camera_service->set_camera(square_entity);

  axgl.run();
  axgl.terminate();
}
