# 8. Model

Models are loaded on demand by `ModelService`, which resolves a key to a set of
meshes, materials, and textures. The raw bytes behind those keys are held by
`ResourceService`.

## ResourceService

`ResourceService` stores resource data by string key:

```cpp
class ResourceService {
public:
  virtual void load_resource(
    const std::string& key, std::span<const std::uint8_t> data
  ) = 0;
  virtual void load_resources(
    std::unordered_map<std::string, std::span<const std::uint8_t>> data
  ) = 0;
  virtual void unload_resource(const std::string& key) = 0;

  [[nodiscard]] virtual bool has_resource(const std::string& key) const = 0;
  virtual const std::span<const std::uint8_t>& get_resource(
    const std::string& key
  ) = 0;
};
```

Models and their textures are installed here first, typically from a generated
resource table or an application-specific bundle, so the model and texture
services can look them up by key.

## ModelService

```cpp
struct ModelResources {
  std::vector<axgl::ptr_t<axgl::Texture>> textures;
  std::vector<axgl::ptr_t<axgl::Material>> materials;
  std::vector<axgl::ptr_t<axgl::component::Mesh>> meshes;
};

virtual ModelResources load_model(const std::string& resource_key) = 0;
```

`load_model` decodes the file at the given resource key and returns the pieces
ready to use. The meshes can be added straight to an entity:

```cpp
const auto resources = axgl.model_service()->load_model("backpack.glb");

const auto entity = axgl.entity_service()->create_entity();
for (const auto& mesh : resources.meshes)
  entity->components().add(mesh);

entity->set_scale(glm::vec3(10.0f));
realm->entities().add(entity);
```

Because the returned meshes already carry their materials and textures, no
manual mesh or material construction is needed. Combine with a light and a
free-fly camera as in [4. Cube](04_cube.md) and [7. Input](07_input.md).

Example: `demo/08_model`.
