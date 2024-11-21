
#include <numbers>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>
#include <axgl/impl/opengl/opengl_service.hpp>
#include <axgl/impl/default/realm_service.hpp>

#include <spdlog/spdlog.h>

void circle_mesh(std::shared_ptr<axgl::interface::Mesh2D> mesh, uint32_t vert_count)
{
  std::vector<axgl::interface::Vertex2D> vertices;
  std::vector<uint32_t> indicies;

  vertices.reserve(vert_count + 1);
  indicies.reserve(vert_count * 3);

  float delta = (2 * std::numbers::pi) / vert_count;
  for (int i = 0; i < vert_count; ++i)
  {
    auto r = i * delta;
    auto x = sin(r) * 0.5f;
    auto y = cos(r) * 0.5f;
    vertices.push_back({ x, y });
    indicies.push_back(i);
    indicies.push_back((i + 1) % vert_count);
    indicies.push_back(vert_count);
  }
  vertices.push_back({ 0, 0 });

  mesh->set_data(vertices);
  mesh->set_indices(indicies);
}

int main()
{
  try
  {
    axgl::Axgl axgl;
    axgl.use_service<axgl::impl::GlfwWindowService>();
    axgl.use_service<axgl::impl::OpenglRendererService>();
    axgl.use_service<axgl::impl::DefaultRealmService>();

    // window
    auto window = axgl.window_service()->create_window();
    window->set_title("Hello circle!");

    // renderer
    auto renderer = axgl.renderer_service()->create_renderer();
    renderer->set_window(window);

    // realm
    auto realm = axgl.realm_service()->create_realm();
    realm->set_renderer(renderer);

    // triangle mesh
    auto mesh = axgl::interface::Component::create_component<axgl::interface::Mesh2D>();
    circle_mesh(mesh, 50);

    // triangle entity
    auto entity = realm->create_entity();
    entity->add_component(mesh);

    // start
    axgl.run();
  }
  catch (const std::exception& e)
  {
    SPDLOG_ERROR(e.what());
  }
}
