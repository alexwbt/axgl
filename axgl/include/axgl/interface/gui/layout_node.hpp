#pragma once

#include <span>

#include <axgl/interface/gui/style.hpp>

namespace axgl::gui
{

// A node in the layout tree. Implemented by gui elements (or any object that
// wants to participate in layout). The engine reads computed style, measures
// children, then writes final geometry back through the setters below.
class LayoutNode
{
public:
  virtual ~LayoutNode() = default;

  // Resolved style used by the engine to pick a layout strategy and read
  // sizing/spacing properties. Const because layout never mutates style.
  [[nodiscard]] virtual const axgl::gui::Style& get_computed_style() const = 0;

  // Final position of the node's top-left corner in page-local coordinates.
  // Set by the engine during the arrange pass; read by the element when
  // drawing and hit-testing.
  [[nodiscard]] virtual glm::vec2 get_position() const = 0;
  virtual void set_position(glm::vec2 position) = 0;

  // Final size (width, height) of the node's content box. May differ from
  // style.get_size() when the parent stretches, shrinks, or clamps the node
  // during arrange.
  [[nodiscard]] virtual glm::vec2 get_size() const = 0;
  virtual void set_size(glm::vec2 size) = 0;

  // Clipping rectangle (left, top, right, bottom) the node should render
  // within. Intersected with the parent's content box by the engine; elements
  // feed it to glScissor.
  [[nodiscard]] virtual glm::vec4 get_scissor_rect() const = 0;
  virtual void set_scissor_rect(glm::vec4 rect) = 0;

  // Dirty flag. True when this node (or a descendant) has changed in a way
  // that requires re-layout: style mutation, content change, child
  // add/remove. Propagates upward so a parent can re-arrange. The page checks
  // the root's flag to decide whether to run the layout pass this frame.
  [[nodiscard]] virtual bool layout_dirty() const = 0;
  virtual void mark_layout_dirty(bool dirty) = 0;

  // Intrinsic content size for the measure pass, ignoring margin/padding and
  // any parent-imposed size. Text elements return their measured text
  // bounds; fixed-size boxes return their style size; empty containers
  // return zero. The engine uses this to satisfy `size: auto` and to size
  // shrink-to-fit flex/block children.
  [[nodiscard]] virtual glm::vec2 measure_intrinsic() const = 0;

  // Direct children as layout nodes, for engine recursion. The returned span
  // is non-owning and only valid while the node's child container is stable
  // (i.e. for the duration of one layout pass). Implementations typically
  // return a cached buffer of LayoutNode* views into their child container to
  // avoid per-pass allocation.
  [[nodiscard]] virtual std::span<axgl::gui::LayoutNode*> layout_children() = 0;
};

} // namespace axgl::gui
