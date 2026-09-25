# 6. Textured Cube

A material can take several textures, one per slot, letting a single mesh use
different maps for color, highlights, surface detail, and displacement. This
topic covers the multi-slot setup and the scale controls that modulate it.

## Texture slots

`Material::add_texture` associates a `Texture` with a `TextureType`:

```cpp
const auto diffuse  = axgl.renderer_service()->create_texture();
diffuse->load_texture(/* ... */);

const auto material = axgl.renderer_service()->create_material("3d");
material->add_texture(axgl::Material::TextureType::kDiffuse,  diffuse);
material->add_texture(axgl::Material::TextureType::kSpecular, specular);
material->add_texture(axgl::Material::TextureType::kNormal,   normal);
material->add_texture(axgl::Material::TextureType::kHeight,   height);
```

| Slot        | Purpose                      |
| ----------- | ---------------------------- |
| `kDiffuse`  | Base color                   |
| `kSpecular` | Specular highlight intensity |
| `kNormal`   | Per-texel surface normal     |
| `kHeight`   | Height/parallax displacement |

A material that only needs color can use `set_color` instead; see
[5. Texture](05_texture.md) and [11. Blending](11_blending.md).

## Scale controls

The normal and height contributions are attenuated per material:

```cpp
material->set_depth_scale(0.05f);  // height/parallax strength
material->set_normal_scale(0.1f);  // normal-map intensity
```

Other per-material sampling controls are `set_tiling` and `set_offset`, plus the
free-form `set_property(key, value)`.

## Applying the material

The material is assigned to a mesh and the entity is placed in a 3D scene with a
light and perspective camera:

```cpp
const auto mesh = axgl.entity_service()->create_component_t<axgl::component::Mesh>();
axgl::mesh::init_cube(*mesh);
mesh->set_material(material);
```

See [4. Cube](04_cube.md) for camera, light, and the custom service that drives
the rotation.

Example: `demo/06_textured_cube`.

