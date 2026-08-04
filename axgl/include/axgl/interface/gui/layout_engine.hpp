#pragma once

#include <span>

#include <axgl/interface/gui/layout_node.hpp>

namespace axgl::gui
{

// Layout strategy that sizes and positions a tree of LayoutNodes. Decoupled
// from Element/Context/Page so different strategies (block flow, flex, grid)
// can be implemented without touching element types, and so the same engine
// works for any backend that supplies LayoutNodes.
class LayoutEngine
{
public:
  virtual ~LayoutEngine() = default;

  // Bottom-up measure pass. Returns the desired size of `node` given the
  // space the parent is willing to give it (width/height). For containers
  // this recursively measures children and aggregates the results according
  // to the strategy (e.g. block stacks heights, flex row sums widths). A
  // fixed-size node ignores `available_size` and returns its intrinsic size.
  // The parent uses the returned value in its arrange pass.
  virtual glm::vec2 measure(axgl::gui::LayoutNode& node, glm::vec2 available_size) const = 0;

  // Top-down arrange pass. Places `node` inside `content_box` (left, top,
  // right, bottom in page-local coordinates) at the given `scale`, then
  // recurses to lay out children within the node's content box (content_box
  // minus padding). Writes final position/size/scissor back through the
  // node. Called by the page traversal after styles have cascaded and before
  // update/render so hit-testing and drawing read settled geometry.
  virtual void layout(axgl::gui::LayoutNode& node, glm::vec4 content_box, float scale) const = 0;
};

} // namespace axgl::gui
