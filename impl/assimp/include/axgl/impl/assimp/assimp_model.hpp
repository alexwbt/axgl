#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/util/string.hpp>
#include <axgl/interface/model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

NAMESPACE_AXGL_IMPL

class AssimpModelService : public interface::ModelService
{
public:
  std::shared_ptr<interface::Entity> load_model(const std::span<const uint8_t> data) override
  {
    Assimp::Importer importer;
    const aiScene* ai_scene = importer.ReadFileFromMemory(data.data(), data.size(), 0);

    return nullptr;
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::AssimpModelService> Axgl::use_service()
{
  auto service = std::make_shared<impl::AssimpModelService>();
  register_service("model", service);

  return service;
}

NAMESPACE_AXGL_END
