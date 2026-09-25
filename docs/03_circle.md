# 3. Circle

Meshes can be built from generated vertex data rather than fixed shapes, and can
share vertices through an index buffer. This topic covers index buffers and the
triangle-fan pattern used to approximate curved shapes.

## Index buffers

`Mesh::set_indices` supplies a `uint32_t` list that indexes into the vertex
array, so each vertex is stored once and reused by multiple triangles:

```cpp
mesh->set_indices(indices);
mesh->set_vertices(vertices);
```

Vertices may be 2D or 3D. Without indices, vertices are consumed in order.

## Triangle fans

A circle is a fan of triangles around a shared center vertex. With `n` rim
vertices plus a center at index `n`:

```cpp
const float delta = 2.0f * pi / n;
for (std::uint32_t i = 0; i < n; ++i) {
  const auto r = static_cast<float>(i) * delta;
  vertices.emplace_back(sin(r), cos(r));
  indices.push_back(i);
  indices.push_back((i + 1) % n);
  indices.push_back(n); // center
}
vertices.emplace_back(0, 0); // center vertex
```

The `(i + 1) % n` wrap closes the ring. The resulting mesh is used exactly like
any other: assign a material, add the mesh component to an entity, and view it
through an orthographic camera (see [2. Triangle](02_triangle.md)).

The same approach produces any convex fan; index buffers are the general tool
for reducing duplicated vertices in arbitrary geometry.

Example: `demo/03_circle`.

