# 11. Blending

Translucent objects require a material with alpha blending enabled; the color's
alpha channel controls the mix. Blended geometry must be drawn after opaque
geometry, which the renderer handles per material.

## The Material interface

`Material` (in `axgl/interface/material.hpp`) exposes the properties that affect
how a mesh is shaded:

```cpp
virtual void set_color(const glm::vec4& color) = 0;
virtual void set_enable_blend(bool enable_blend) = 0;
virtual void set_enable_depth_test(bool enable_depth_test) = 0;
virtual void set_alpha_discard(float alpha_discard) = 0;
virtual void set_tiling(glm::vec2 tiling) = 0;
virtual void set_offset(glm::vec2 offset) = 0;
virtual void set_depth_scale(float depth_scale) = 0;
virtual void set_normal_scale(float normal_scale) = 0;
virtual void set_property(const std::string& key, const std::string& value) = 0;
virtual void add_texture(TextureType type, ptr_t<Texture> texture) = 0;
```

## Alpha blending

```cpp
const auto material = axgl.renderer_service()->create_material("3d");
material->set_color({1.0f, 0.0f, 0.0f, 0.5f}); // alpha = 0.5
material->set_enable_blend(true);
```

- `set_color` takes a `glm::vec4`; the fourth component is opacity.
- `set_enable_blend(true)` enables alpha blending for the material.
- `set_enable_depth_test(false)` is often used together with blending for
  effects that should not occlude, as in [9. Text](09_text.md).

## Material properties

`set_property(key, value)` passes string key/value settings through to the
shading backend, for options that do not have a dedicated setter:

```cpp
material->set_property("shininess", "0");
```

## Drawing order

Blended and opaque geometry need different handling: opaque geometry writes
depth, while blended geometry must be drawn afterward and sorted back-to-front.
Assigning the correct material flags is what lets the renderer choose the right
pass, so a solid cube and translucent ones can coexist in one scene. The rest of
the scene — light, camera, entity placement — is covered in
[4. Cube](04_cube.md).

Example: `demo/11_blending`.
