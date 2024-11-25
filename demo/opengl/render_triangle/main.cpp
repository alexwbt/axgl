
#include <numbers>

#include <axgl/axgl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>
#include <axgl/impl/opengl/opengl_service.hpp>

#include <spdlog/spdlog.h>

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
    window->set_title("Hello triangle!");

    // renderer
    auto renderer = axgl.renderer_service()->create_renderer();
    renderer->set_window(window);

    // realm
    auto realm = axgl.realm_service()->create_realm();
    realm->set_renderer(renderer);

    // triangle mesh
    auto mesh = axgl.create_component<axgl::interface::Mesh2D>();
    mesh->set_vertices({ {-450.0f, -300.0f}, {0.0f, 310.0f}, {450.0f, -300.0f} });
    mesh->set_color({ 1.0, 0.5, 0.2 });

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
