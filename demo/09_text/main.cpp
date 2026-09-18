#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
  #include <opengl/text.hpp>
#endif
#include <axgl/common/mesh.hpp>
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>

#include <demo_text/res.hpp>

class Application final : public axgl::impl::ServiceBase {
public:
  static constexpr std::string_view kTypeId = "app";

private:
  axgl::ptr_t<axgl::Realm> realm_;
  axgl::ptr_t<axgl::Entity> text_entity_;

  bool border_ = false;
  axgl::ptr_t<axgl::Input> border_input_;

  static constexpr const char8_t* kText
    = u8"axgl text service\n"
      u8"  - Multi-font fallback (arial + noto-tc)\n"
      u8"  - Word wrap with CJK-aware line breaking\n"
      u8"  - Newline support, leading space trimming\n"
      u8"  - Symbols: !@#$%^&*()[]{}<>,.`~-_+=\\/|?'\":;\n"
      u8"\n"
      u8"axgl is a C++ game development framework with a service-oriented "
      u8"core over a component-based entity/realm tree. The text service "
      u8"renders text to a texture via FreeType, supporting font fallback, "
      u8"word wrapping, and mixed CJK + Latin layout.\n"
      u8"\n"
      u8"axgl 文字服務\n"
      u8"  - 多字體後備（arial 與 noto-tc）\n"
      u8"  - 自動換行，支援中日韓斷行\n"
      u8"  - 支援換行符號，並修剪行首空白\n"
      u8"  - 符號測試：「」【】《》、。；：！？\n"
      u8"\n"
      u8"axgl 是一個 C++ 遊戲開發框架，採用服務導向的核心架構，建立在元件為基礎"
      u8"的實體與領域樹之上。文字服務透過 FreeType 將文字渲染成材質，支援字體後"
      u8"備、自動換行，以及中日韓與拉丁文字混合排版。\n"
      u8"\n"
      u8"Mixed 中英 layout 測試 test 測試 123 mixed\n"
      u8"  - The TextService TextService renders text to a texture via "
      u8"FreeType.\n"
      u8"  - 文字服務 TextService 透過 FreeType 渲染文字材質。\n"
      u8"  - English 中文 English 中文 symbols 「」【】$@#%&!\n"
      u8"\n"
      u8"axgl 的 Renderer 將 opaque、transparent、shadow 等 pass 組合起來，"
      u8"text texture 作為 2d material 的 diffuse texture 被 render 到 "
      u8"screen framebuffer。Mixed 測試：core service 實體 entity 領域 realm "
      u8"元件 component 材質 material 著色器 shader buffer vertex fragment "
      u8"「文字服務」TextService、『渲染器』Renderer、《領域》Realm。";

public:
  void on_start() override {
    const auto& window_service = axgl_->window_service();
    const auto& input_service = axgl_->input_service();
    const auto& renderer_service = axgl_->renderer_service();
    const auto& realm_service = axgl_->realm_service();
    const auto& entity_service = axgl_->entity_service();
    const auto& camera_service = axgl_->camera_service();
    const auto& text_service = axgl_->text_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello text!");

    // input
    input_service->set_window(window);
    border_input_ = axgl::create_ptr<axgl::Input>(
      "Toggle Border", axgl::Input::Source::kKeyB
    );
    input_service->add_input(border_input_);

    // renderer
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);
    renderer->set_enable_msaa(true);
    renderer->set_msaa_sample_count(8);
    renderer_service->set_active_renderer(renderer);

    // realm
    realm_ = realm_service->create_realm();
    realm_service->set_active_realm(realm_);

    // load fonts
    text_service->load_font("arial", demo_text_res::get("font/arial.ttf"), 0);
    text_service->load_font(
      "noto-tc", demo_text_res::get("font/noto-tc.ttf"), 0
    );

    create_camera();
    create_light();
    create_text();
  }

  void update() override {
    if (border_input_->clicked()) {
      border_ = !border_;
      create_text();
    }
  }

private:
  void create_text() {
    const auto& renderer_service = axgl_->renderer_service();
    const auto& entity_service = axgl_->entity_service();
    const auto text_service
      = axgl_->get_service_t<axgl::impl::opengl::TextService>();

    const auto texture = text_service->create_texture(
      reinterpret_cast<const char*>(kText),
      {"arial", "noto-tc"},
      ::opengl::TextOptions{
        .color = {1.0f, 0.5f, 0.2f, 1.0f},
        .size = 48,
        .max_width = 1024,
        .wrap = ::opengl::WrapMode::Auto,
      }
    );

    // material
    const auto material = renderer_service->create_material("2d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, texture);
    // mesh
    const auto mesh
      = entity_service->create_component_t<axgl::component::Mesh>();
    axgl::mesh::init_quad(*mesh);
    mesh->set_material(material);
    // entity
    if (text_entity_) realm_->entities().remove(text_entity_);
    text_entity_ = entity_service->create_entity();
    text_entity_->add_component(mesh);
    text_entity_->set_scale(
      glm::vec3(texture->get_width(), texture->get_height(), 1.0f) * 0.01f
    );
    if (border_) create_border();

    realm_->entities().add(text_entity_);
  }

  void create_border() {
    const auto& renderer_service = axgl_->renderer_service();
    const auto& entity_service = axgl_->entity_service();

    const auto material = renderer_service->create_material("color");
    material->set_color({0.0f, 1.0f, 0.0f, 1.0f});
    material->set_enable_depth_test(false);

    const auto mesh
      = entity_service->create_component_t<axgl::component::Mesh>();
    axgl::mesh::init_quad(*mesh);
    mesh->set_draw_mode(axgl::component::Mesh::DrawMode::kLineLoop);
    mesh->set_material(material);

    const auto border_entity = entity_service->create_entity();
    border_entity->add_component(mesh);
    text_entity_->add_child(border_entity);
  }

  void create_camera() {
    const auto& camera_service = axgl_->camera_service();
    const auto& entity_service = axgl_->entity_service();

    const auto camera_entity = entity_service->create_entity();
    const auto camera_comp
      = entity_service->create_component_t<axgl::impl::component::Camera>();
    camera_entity->add_component(camera_comp);
    realm_->entities().add(camera_entity);
    camera_entity->transform().position.z = -2;
    camera_entity->transform().update_matrix();

    // camera input
    camera_service->set_camera_mode(
      axgl::create_ptr<axgl::impl::camera::Keyboard3DFreeFlyCameraMode>()
    );
    camera_service->set_camera_entity(camera_entity);
  }

  void create_light() {
    const auto& entity_service = axgl_->entity_service();
    const auto light_entity = entity_service->create_entity();
    const auto light_comp
      = entity_service->create_component_t<axgl::impl::component::Light>();
    light_comp->light = axgl::Light::sunlight({0.2f, -1.0f, 1.2f});
    light_entity->add_component(light_comp);
    realm_->entities().add(light_entity);
  }
};

int main() {
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
