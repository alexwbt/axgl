#pragma once

#include <span>
#include <string>
#include <vector>
#include <memory>
#include <ranges>
#include <cstdlib>
#include <filesystem>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/resource.hpp>
#include <axgl/interface/component/mesh.hpp>

NAMESPACE_AXGL_IMPL

class AssimpModelService;

class ModelLoader
{
  friend class AssimpModelService;

  std::shared_ptr<interface::RealmService> realm_service_;
  std::shared_ptr<interface::RendererService> renderer_service_;
  std::shared_ptr<interface::ResourceService> resource_service_;

  std::string resource_key_;
  std::string material_type_;
  std::vector<std::shared_ptr<interface::Texture>> embedded_textures_;

  interface::ModelService::ModelResources resources_;

  ModelLoader(
    std::shared_ptr<interface::RealmService> realm_service,
    std::shared_ptr<interface::RendererService> renderer_service,
    std::shared_ptr<interface::ResourceService> resource_service,
    std::shared_ptr<interface::Entity> entity,
    const std::string& resource_key,
    const std::string& material_type
  ) :
    realm_service_(std::move(realm_service)),
    renderer_service_(std::move(renderer_service)),
    resource_service_(std::move(resource_service)),
    resource_key_(resource_key),
    material_type_(material_type)
  {
    Assimp::Importer importer;
    const auto& data = resource_service_->get_resource(resource_key);
    const auto* ai_scene = importer.ReadFileFromMemory(data.data(), data.size(), 0);
#ifdef AXGL_DEBUG
    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
    {
      SPDLOG_ERROR("Failed to load model: {}", importer.GetErrorString());
      throw std::runtime_error(importer.GetErrorString());
    }
#endif
    if (ai_scene->HasTextures())
    {
      embedded_textures_.resize(ai_scene->mNumTextures);
      for (int i = 0; i < ai_scene->mNumTextures; ++i)
      {
        embedded_textures_[i] = renderer_service_->create_texture();
        embedded_textures_[i]->load_texture({
          reinterpret_cast<uint8_t*>(ai_scene->mTextures[i]->pcData),
          ai_scene->mTextures[i]->mWidth });

        resources_.textures.push_back(embedded_textures_[i]);
      }
    }
    process_node(entity, ai_scene->mRootNode, ai_scene);
  }

  void process_node(
    std::shared_ptr<interface::Entity> entity,
    aiNode* ai_node, const aiScene* ai_scene)
  {
    for (int i = 0; i < ai_node->mNumMeshes; ++i)
    {
      aiMesh* ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
      entity->add_component(load_mesh(ai_mesh, ai_scene));
    }

    // add children node mesh
    for (int i = 0; i < ai_node->mNumChildren; ++i)
      process_node(entity, ai_node->mChildren[i], ai_scene);
  }

  std::shared_ptr<axgl::interface::Mesh> load_mesh(aiMesh* ai_mesh, const aiScene* ai_scene)
  {
    auto mesh = realm_service_->create_component<axgl::interface::Mesh>();
    resources_.meshes.push_back(mesh);

    std::vector<glm::vec3> vertices;
    vertices.reserve(ai_mesh->mNumVertices);
    for (int i = 0; i < ai_mesh->mNumVertices; ++i)
      vertices.emplace_back(
        ai_mesh->mVertices[i].x,
        ai_mesh->mVertices[i].y,
        ai_mesh->mVertices[i].z);
    mesh->set_vertices(vertices);

    if (ai_mesh->HasNormals())
    {
      std::vector<glm::vec3> normals;
      normals.reserve(ai_mesh->mNumVertices);
      for (int i = 0; i < ai_mesh->mNumVertices; ++i)
        normals.emplace_back(
          ai_mesh->mNormals[i].x,
          ai_mesh->mNormals[i].y,
          ai_mesh->mNormals[i].z);
      mesh->set_normals(normals);
    }

    if (ai_mesh->HasTextureCoords(0))
    {
      std::vector<glm::vec2> uv;
      uv.reserve(ai_mesh->mNumVertices);
      for (int i = 0; i < ai_mesh->mNumVertices; ++i)
        uv.emplace_back(
          ai_mesh->mTextureCoords[0][i].x,
          ai_mesh->mTextureCoords[0][i].y);
      mesh->set_uv(uv);
    }

    if (ai_mesh->HasFaces())
    {
      std::vector<uint32_t> indices;
      for (int i = 0; i < ai_mesh->mNumFaces; ++i)
        for (int j = 0; j < ai_mesh->mFaces[i].mNumIndices; ++j)
          indices.push_back(ai_mesh->mFaces[i].mIndices[j]);
      mesh->set_indices(indices);
    }

    auto material = renderer_service_->create_material(material_type_);
    aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
    for (int i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; ++i)
    {
      auto ai_texture_type = static_cast<aiTextureType>(i);
      auto texture_type = map_texture_type(ai_texture_type);

      if (texture_type == interface::TextureType::kUnknown)
        continue;

      load_textures(ai_material, ai_texture_type, material, texture_type);
    }
    mesh->set_material(material);
    resources_.materials.push_back(material);

    return mesh;
  }

  void load_textures(
    aiMaterial* ai_material,
    aiTextureType ai_texture_type,
    std::shared_ptr<interface::Material> material,
    interface::TextureType texture_type
  )
  {
    auto count = ai_material->GetTextureCount(ai_texture_type);
    for (int i = 0; i < count; ++i)
    {
      aiString texture_path;
      ai_material->GetTexture(ai_texture_type, i, &texture_path);

      if (*texture_path.C_Str() == '*')
      {
        int index = std::atoi(texture_path.C_Str() + 1);
#ifdef AXGL_DEBUG
        if (index < 0 || index > embedded_textures_.size())
          throw std::runtime_error("Invalid texture path.");
#endif
        material->add_texture(texture_type, embedded_textures_[index]);
      }
      else
      {
        std::filesystem::path base_path = std::filesystem::path(resource_key_);
        std::filesystem::path relative_path = std::filesystem::path(texture_path.C_Str());

        std::string path = (base_path.parent_path() / relative_path).string();
        std::ranges::replace(path, '\\', '/');

        auto texture = renderer_service_->create_texture();
        texture->load_texture(resource_service_->get_resource(path));
        material->add_texture(texture_type, std::move(texture));
        resources_.textures.push_back(texture);
      }
      }
    }

  interface::TextureType map_texture_type(aiTextureType ai_texture_type)
  {
    using enum interface::TextureType;
    switch (ai_texture_type)
    {
    case aiTextureType_DIFFUSE: return kDiffuse;
    case aiTextureType_SPECULAR: return kSpecular;
      // case aiTextureType_AMBIENT: return kAmbient;
      // case aiTextureType_EMISSIVE: return kEmissive;
    case aiTextureType_HEIGHT: return kHeight;
    case aiTextureType_NORMALS: return kNormal;
      // case aiTextureType_SHININESS: return kShininess;
      // case aiTextureType_OPACITY: return kOpacity;
      // case aiTextureType_DISPLACEMENT: return kDisplacement;
      // case aiTextureType_LIGHTMAP: return kLightmap;
      // case aiTextureType_REFLECTION: return kReflection;
      // case aiTextureType_BASE_COLOR: return kBase_color;
      // case aiTextureType_NORMAL_CAMERA: return kNormal_camera;
      // case aiTextureType_EMISSION_COLOR: return kEmission_color;
      // case aiTextureType_METALNESS: return kMetalness;
      // case aiTextureType_DIFFUSE_ROUGHNESS: return kDiffuse_roughness;
      // case aiTextureType_AMBIENT_OCCLUSION: return kAmbient_occlusion;
      // case aiTextureType_SHEEN: return kSheen;
      // case aiTextureType_CLEARCOAT: return kClearcoat;
      // case aiTextureType_TRANSMISSION: return kTransmission;
    default: return kUnknown;
    }
  }
  };

NAMESPACE_AXGL_IMPL_END
