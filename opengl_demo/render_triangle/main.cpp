
#include <numbers>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>
#include <axgl/impl/opengl/opengl_service.hpp>
#include <axgl/impl/default/realm_service.hpp>

#include <spdlog/spdlog.h>

int main()
{
  try
  {
    axgl::Axgl axgl;
    axgl.use_service<axgl::impl::GlfwWindowService>();
    // axgl.use_service<axgl::impl::OpenglRendererService>();
    axgl.use_service<axgl::impl::DefaultRealmService>();

    // window
    auto window = axgl.window_service()->create_window();
    window->set_title("Hello triangle!");

    // realm
    auto realm = axgl.realm_service()->create_realm();

    //
    auto entity = realm->create_entity();

    // triangle mesh
    auto mesh = axgl.renderer_service()->create_mesh2d();

    // generate vertices
    std::vector<axgl::interface::Vertex2D> vertices(3);
    double d = (2 * std::numbers::pi) / vertices.size();
    for (int i = 0; i < vertices.size(); i++)
    {
      double r = i * d;
      vertices[i].x = sin(r);
      vertices[i].y = cos(r);
    }
    mesh->set_data(vertices);

    // start
    axgl.run();
  }
  catch (const std::exception& e)
  {
    SPDLOG_ERROR(e.what());
  }
}
