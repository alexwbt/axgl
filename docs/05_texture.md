# 5. Texture

Textures are created by the `RendererService`, filled with raw byte data, and
attached to materials. A mesh samples them through its UV coordinates.

## The Texture interface

```cpp
class Texture {
public:
  virtual void load_texture(std::span<const std::uint8_t> data) = 0;
  [[nodiscard]] virtual std::uint32_t get_width() = 0;
  [[nodiscard]] virtual std::uint32_t get_height() = 0;
};
```

`load_texture` takes the encoded image bytes; `get_width` / `get_height` report
the decoded size and are useful for sizing geometry, as in
[9. Text](09_text.md).

## Loading and assigning

```cpp
const auto texture = axgl.renderer_service()->create_texture();
texture->load_texture(demo_texture_res::get("container.png"));

const auto material = axgl.renderer_service()->create_material("2d");
material->add_texture(axgl::Material::TextureType::kDiffuse, texture);
```

`Material::TextureType` has `kDiffuse`, `kSpecular`, `kNormal`, `kHeight`, and
`kUnknown`. Which slots a material actually uses depends on its type.

## UV mapping

A mesh samples a texture using its UV coordinates. A quad needs four UVs and two
triangles:

```cpp
mesh->set_vertices(std::vector<glm::vec2>{
  {0.5f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f}, {-0.5f, 0.5f}
});
mesh->set_uv(std::vector<glm::vec2>{
  {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}
});
mesh->set_indices(std::vector<std::uint32_t>{0, 1, 2, 0, 2, 3});
mesh->set_material(material);
```

`axgl::mesh::init_quad` (in `<axgl/common/mesh.hpp>`) produces exactly this
vertex/UV/index layout. The orthographic camera and scale follow
[2. Triangle](02_triangle.md); for multiple texture inputs at once, see
[6. Textured Cube](06_textured_cube.md).

Example: `demo/05_texture`.

