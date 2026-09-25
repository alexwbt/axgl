# 9. Text

`TextService` turns a string into a `Texture` using loaded fonts. The texture
can then be used anywhere a texture is accepted, such as the diffuse slot of a
2D material.

## Loading fonts

```cpp
axgl.text_service()->load_font("arial",   /* font bytes */, 0);
axgl.text_service()->load_font("noto-tc", /* font bytes */, 0);
```

Fonts are registered by name; the third argument is the face index within the
file. `unload_font` removes one. Text can request a list of fonts for fallback,
so glyphs missing from the first are drawn from the next.

## Rendering to a texture

```cpp
struct Options {
  const std::string& value;
  const std::vector<std::string>& fonts;
  const glm::vec4& font_color;
  float font_size = 16.0f;
  std::int32_t max_width = -1;
  std::int32_t max_height = -1;
  float line_height = 0.0f;
  Wrap wrap = Wrap::Auto;
  bool vertical = false;
};

virtual axgl::ptr_t<axgl::Texture> create_texture(const Options&) const = 0;
```

```cpp
const auto texture = axgl.text_service()->create_texture({
  .value = text,
  .fonts = {"arial", "noto-tc"},
  .font_color = {1.0f, 0.5f, 0.2f, 1.0f},
  .font_size = 48.0f,
  .max_width = 1024,
  .wrap = axgl::TextService::Wrap::Auto,
});
```

`Wrap` is `None`, `Word`, `Char`, or `Auto`. The returned texture's `get_width`
/ `get_height` give the measured size, which is useful when sizing the quad that
displays it.

## Displaying the text

Treat the text texture like any other: put it on a 2D material and a quad.

```cpp
const auto material = axgl.renderer_service()->create_material("2d");
material->add_texture(axgl::Material::TextureType::kDiffuse, texture);

const auto mesh = axgl.entity_service()->create_component_t<axgl::component::Mesh>();
axgl::mesh::init_quad(*mesh);
mesh->set_material(material);
```

To outline or decorate text, add a child entity to the text entity with a
different mesh/material. `Entity::add_child` makes the child follow the parent's
transform:

```cpp
mesh->set_draw_mode(axgl::component::Mesh::DrawMode::kLineLoop);
material->set_enable_depth_test(false);
text_entity->add_child(border_entity);
```

Example: `demo/09_text`.

