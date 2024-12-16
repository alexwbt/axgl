#pragma once

#include <span>

#include <axgl/axgl.hpp>
#include <axgl/except.hpp>
#include <axgl/namespace.hpp>
#include <axgl/util/string.hpp>
#include <axgl/interface/model.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/resource.hpp>
#include <axgl/interface/component/mesh.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#ifndef DEFINED_COMPONENT_MESH
#error Implementation of interface::Mesh must be defined before importing <axgl/impl/assimp/model.hpp>
#endif

NAMESPACE_AXGL_IMPL

class AssimpModelService : public interface::ModelService
{
private:
  std::shared_ptr<interface::RealmService> realm_service_;
  std::shared_ptr<interface::RendererService> renderer_service_;
  std::shared_ptr<interface::ResourceService> resource_service_;

public:
  void initialize() override
  {
    auto context = get_context();
    realm_service_ = context->axgl->realm_service();
    renderer_service_ = context->axgl->renderer_service();
    resource_service_ = context->axgl->resource_service();
  }

  void load_model(std::shared_ptr<interface::Component> root, const std::string& resource_key) override
  {
    auto data = resource_service_->get_resource(resource_key);

    Assimp::Importer importer;
    const aiScene* ai_scene = importer.ReadFileFromMemory(data.data(), data.size(),
      aiProcess_Triangulate |
      aiProcess_GenSmoothNormals |
      aiProcess_CalcTangentSpace);

#ifdef AXGL_DEBUG
    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
    {
      SPDLOG_ERROR("Failed to load model: {}", importer.GetErrorString());
      throw std::runtime_error(importer.GetErrorString());
    }
#endif

    process_node(root, ai_scene->mRootNode, ai_scene);
  }

private:
  void process_node(
    std::shared_ptr<interface::Component> root,
    aiNode* ai_node, const aiScene* ai_scene)
  {
    for (int i = 0; i < ai_node->mNumMeshes; ++i)
    {
      aiMesh* mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
      root->add_component(load_mesh(mesh, ai_scene));
    }

    // add children node mesh
    for (int i = 0; i < ai_node->mNumChildren; ++i)
      process_node(root, ai_node->mChildren[i], ai_scene);
  }

  std::shared_ptr<axgl::interface::Mesh> load_mesh(aiMesh* ai_mesh, const aiScene* ai_scene)
  {
    auto mesh = realm_service_->create_component<axgl::interface::Mesh>();

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

    auto material = renderer_service_->create_material("default");
    aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
    for (int i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; ++i)
    {
      auto type = static_cast<aiTextureType>(i);
      load_textures(ai_material, type, material, map_texture_type(type));
    }
    mesh->set_material(material);

    return mesh;
  }

  void load_textures(
    aiMaterial* ai_material,
    aiTextureType ai_texture_type,
    std::shared_ptr<interface::Material> material,
    interface::Texture::Type texture_type)
  {
    for (int i = 0; i < ai_material->GetTextureCount(ai_texture_type); ++i)
    {
      aiString str;
      ai_material->GetTexture(ai_texture_type, i, &str);

      auto texture = renderer_service_->create_texture();

      SPDLOG_INFO("texture: {}", str.C_Str());
      // texture->load_texture(texture_type, {});
      material->add_texture(texture);
    }
  }

  interface::Texture::Type map_texture_type(aiTextureType ai_texture_type)
  {
    using enum interface::Texture::Type;
    switch (ai_texture_type)
    {
    case aiTextureType_DIFFUSE: return DIFFUSE;
    case aiTextureType_SPECULAR: return SPECULAR;
      // case aiTextureType_AMBIENT: return AMBIENT;
      // case aiTextureType_EMISSIVE: return EMISSIVE;
    case aiTextureType_HEIGHT: return HEIGHT;
    case aiTextureType_NORMALS: return NORMAL;
      // case aiTextureType_SHININESS: return SHININESS;
      // case aiTextureType_OPACITY: return OPACITY;
      // case aiTextureType_DISPLACEMENT: return DISPLACEMENT;
      // case aiTextureType_LIGHTMAP: return LIGHTMAP;
      // case aiTextureType_REFLECTION: return REFLECTION;
      // case aiTextureType_BASE_COLOR: return BASE_COLOR;
      // case aiTextureType_NORMAL_CAMERA: return NORMAL_CAMERA;
      // case aiTextureType_EMISSION_COLOR: return EMISSION_COLOR;
      // case aiTextureType_METALNESS: return METALNESS;
      // case aiTextureType_DIFFUSE_ROUGHNESS: return DIFFUSE_ROUGHNESS;
      // case aiTextureType_AMBIENT_OCCLUSION: return AMBIENT_OCCLUSION;
      // case aiTextureType_SHEEN: return SHEEN;
      // case aiTextureType_CLEARCOAT: return CLEARCOAT;
      // case aiTextureType_TRANSMISSION: return TRANSMISSION;
    default: return UNKNOWN;
    }
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
