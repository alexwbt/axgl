#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif
#include <axgl/common/mesh.hpp>

#include <demo_textured_cube/res.hpp>

class Application final : public axgl::impl::ServiceBase
{
public:
  static constexpr std::string_view kTypeId = "app";

private:
  std::shared_ptr<axgl::Entity> cube_entity_;

public:
  void on_start() override
  {
    const auto& window_service = axgl_->window_service();
    const auto& renderer_service = axgl_->renderer_service();
    const auto& realm_service = axgl_->realm_service();
    const auto& entity_service = axgl_->entity_service();
    const auto& camera_service = axgl_->camera_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello textured cube!");

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
      cube_entity_ = entity_service->create_entity();
      // diffuse texture
      const auto diffuse_texture = renderer_service->create_texture();
      diffuse_texture->load_texture(
        demo_textured_cube_res::get("concrete/diffuse.png")
      );
      // specular texture
      const auto specular_texture = renderer_service->create_texture();
      specular_texture->load_texture(
        demo_textured_cube_res::get("concrete/specular.png")
      );
      // normal texture
      const auto normal_texture = renderer_service->create_texture();
      normal_texture->load_texture(
        demo_textured_cube_res::get("concrete/normal.png")
      );
      // height texture
      const auto height_texture = renderer_service->create_texture();
      height_texture->load_texture(
        demo_textured_cube_res::get("concrete/height.png")
      );

      // material
      const auto material = renderer_service->create_material("3d");
      material->add_texture(
        axgl::Material::TextureType::kDiffuse, diffuse_texture
      );
      material->add_texture(
        axgl::Material::TextureType::kSpecular, specular_texture
      );
      material->add_texture(
        axgl::Material::TextureType::kNormal, normal_texture
      );
      material->add_texture(
        axgl::Material::TextureType::kHeight, height_texture
      );
      material->set_depth_scale(0.05f);
      material->set_normal_scale(0.1f);

      // square mesh
      const auto mesh
        = entity_service->create_component_t<axgl::component::Mesh>();
      axgl::mesh::init_cube(*mesh);
      mesh->set_material(material);
      cube_entity_->add_component(mesh);
      realm->entities().add(cube_entity_);
    }
  }

  void tick() override
  {
    auto& transform = cube_entity_->transform();
    transform.rotation += glm::vec3(1.0f, 2.0f, 5.0f) * 0.0005f;
    transform.update_matrix();
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl::configure_default(axgl);
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
#endif
  axgl.register_service_t<Application>();
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
