
static std::vector<axgl::opengl::shader::PhongShader::Vertex> cube_vertices = {
  // Back face
  { glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // Bottom-left
  { glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(1.0f, 1.0f, 1.0f) }, // bottom-right
  { glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // top-right
  { glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // top-right
  { glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(0.0f, 0.0f, 0.0f) }, // top-left
  { glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // bottom-left
  // Front face
  { glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // bottom-left
  { glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // top-right
  { glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(1.0f, 1.0f, 1.0f) }, // bottom-right
  { glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // top-right
  { glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // bottom-left
  { glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(0.0f, 0.0f, 0.0f) }, // top-left
  // Left face
  { glm::vec3(-0.5f,  0.5f,  0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, 0.0f, 0.0f) }, // top-right
  { glm::vec3(-0.5f, -0.5f, -0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(1.0f, 1.0f, 1.0f) }, // bottom-left
  { glm::vec3(-0.5f,  0.5f, -0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // top-left
  { glm::vec3(-0.5f, -0.5f, -0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(1.0f, 1.0f, 1.0f) }, // bottom-left
  { glm::vec3(-0.5f,  0.5f,  0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, 0.0f, 0.0f) }, // top-right
  { glm::vec3(-0.5f, -0.5f,  0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // bottom-right
  // Right face
  { glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, 0.0f, 0.0f) }, // top-left
  { glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // top-right
  { glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(1.0f, 1.0f, 1.0f) }, // bottom-right
  { glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(1.0f, 1.0f, 1.0f) }, // bottom-right
  { glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // bottom-left
  { glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, 0.0f, 0.0f) }, // top-left
  // Bottom face
  { glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // top-right
  { glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // bottom-left
  { glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(1.0f, 1.0f, 1.0f) }, // top-left
  { glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // bottom-left
  { glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // top-right
  { glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(0.0f, 0.0f, 0.0f) }, // bottom-right
  // Top face
  { glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }, // top-left
  { glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(1.0f, 1.0f, 1.0f) }, // top-right
  { glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // bottom-right
  { glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) }, // bottom-right
  { glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(0.0f, 0.0f, 0.0f) }, // bottom-left
  { glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(0.0f, 1.0f, 1.0f) }  // top-left
};

class Cube : public axgl::world::Entity
{
  std::shared_ptr<axgl::opengl::Model> model_;

public:
  Cube() :
    model_(std::make_shared<axgl::opengl::Model>(
      cube_vertices,
      get_shader(ShaderID::kPhong),
      nullptr
    ))
  {}

  void render(const axgl::world::RenderContext& context)
  {
    using namespace axgl::opengl::shader;

    PhongShader::LightColor light_color{ glm::vec3(0.3f), glm::vec3(1.0f), glm::vec3(1.0f) };

    std::vector<std::shared_ptr<PhongShader::Light>> lights = {
      std::make_shared<PhongShader::Light>(glm::vec3(0.2f, -1.0f, 1.2f), light_color)
    };

    PhongShader::Uniforms uniforms;
    uniforms.lights = lights;
    uniforms.camera_pos = context.camera->position;
    uniforms.model = model_->model_matrix();
    uniforms.mvp = context.pv * uniforms.model;
    uniforms.diffuse_map = 0;
    uniforms.shininess = 16.0f;
    uniforms.specular = 0.2f;

    model_->render(uniforms);
  }

};
