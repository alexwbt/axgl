#include <assimp/postprocess.h>

class Backpack : public axgl::world::Entity
{
  std::shared_ptr<axgl::opengl::shader::PhongShader> shader_;
  std::shared_ptr<axgl::opengl::Model> model_;

public:
  Backpack() :
    shader_(get_shader<axgl::opengl::shader::PhongShader>(ShaderID::kTexturePhong)),
    model_(axgl::opengl::Model::load_model(
      "res/models/backpack/backpack.obj",
      aiProcess_Triangulate |
      aiProcess_GenSmoothNormals |
      aiProcess_CalcTangentSpace))
  {
    model_->translation.x += 10;
  }

  void render(const axgl::world::RenderContext& context)
  {
    using namespace axgl::opengl::shader;

    axgl::world::LightColor light_color{ glm::vec3(0.3f), glm::vec3(1.0f), glm::vec3(1.0f) };

    std::vector<std::shared_ptr<axgl::world::Light>> lights = {
      std::make_shared<axgl::world::Light>(glm::vec3(0.2f, -1.0f, 1.2f), light_color)
    };

    PhongShader::Uniforms uniforms;
    uniforms.lights = lights;
    uniforms.model = model_->model_matrix();
    uniforms.mvp = context.pv * uniforms.model;
    uniforms.camera_pos = context.camera->position;
    uniforms.shininess = 16.0f;
    uniforms.specular = 0.2f;

    shader_->use_program();
    shader_->use_uniforms(uniforms);

    model_->render(shader_);
  }

};
