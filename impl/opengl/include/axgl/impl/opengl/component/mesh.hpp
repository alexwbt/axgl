#pragma once

#include "axgl/axgl.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/component/mesh.hpp"

NAMESPACE_AXGL_IMPL

class OpenglMesh2D : public interface::Mesh2D
{
public:
  void update() override {}

  void render() const override
  {
    // glBindVertexArray(vao_id_);
    // switch (type_)
    // {
    // case Type::kVertex:
    //   glDrawArrays(mode_, 0, vertices_size_);
    //   break;
    // case Type::kElement:
    //   glDrawElements(mode_, indices_size_, GL_UNSIGNED_INT, 0);
    //   break;
    // }
  }

  void set_data(const std::vector<interface::Vertex2D>& data) override
  {

  }

  void set_indices(const std::vector<uint32_t>& indices) override
  {

  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL_INTERFACE

template<>
std::shared_ptr<Mesh2D> Component::create_component()
{
  return std::make_shared<impl::OpenglMesh2D>();
}

NAMESPACE_AXGL_INTERFACE_END
