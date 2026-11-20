#pragma once

#include <string>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/services/gui_service.hpp>

#include <axgl/impl/service_base.hpp>
#include <util/string.hpp>

namespace axgl::impl {

class GuiServiceBase : virtual public axgl::GuiService,
                       public axgl::impl::ServiceBase {
protected:
  std::unordered_map<std::string, axgl::ptr_t<axgl::gui::Page>> pages_;
  std::string active_path_ = "/";
  axgl::ptr_t<axgl::gui::Page> active_page_;
  bool active_initialized_ = false;

public:
  void initialize() override {
    const auto text_style = create_style("text");

    create_style("header1")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_font_size(32.0f)
      ->set_margin(glm::vec4(10.0f));

    create_style("paragraph")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_margin(glm::vec4(10.0f));

    create_style("button")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kInline)
      ->set_padding(glm::vec4(10.0f))
      ->set_margin(glm::vec4(10.0f))
      ->set_color({1.0f, 1.0f, 1.0f, 0.2f});
    create_style("button:hover")
      ->set_cursor(axgl::gui::Cursor::kPointer)
      ->set_color({1.0f, 1.0f, 1.0f, 0.5f});
    create_style("button:active")->set_color({1.0f, 1.0f, 1.0f, 0.8f});

    create_style("input")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_padding(glm::vec4(6.0f))
      ->set_margin(glm::vec4(10.0f))
      ->set_color({0.0f, 0.0f, 0.0f, 0.4f});
    create_style("input:hover")
      ->set_cursor(axgl::gui::Cursor::kText)
      ->set_color({0.0f, 0.0f, 0.0f, 0.5f});
    create_style("input:focus")->set_color({0.0f, 0.0f, 0.0f, 0.7f});

    create_style("toggle")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_padding(glm::vec4(4.0f))
      ->set_margin(glm::vec4(4.0f, 10.0f, 4.0f, 10.0f));
    create_style("toggle:hover")->set_cursor(axgl::gui::Cursor::kPointer);
    create_style("toggle:focus")->set_color({1.0f, 1.0f, 1.0f, 0.1f});

    create_style("slider")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_padding(glm::vec4(12.0f))
      ->set_margin(glm::vec4(4.0f, 10.0f, 4.0f, 10.0f));
    create_style("slider:hover")
      ->set_cursor(axgl::gui::Cursor::kResizeHorizontal);
    create_style("slider:focus")->set_color({1.0f, 1.0f, 1.0f, 0.1f});

    create_style("dropdown")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_padding(glm::vec4(6.0f))
      ->set_margin(glm::vec4(4.0f, 10.0f, 4.0f, 10.0f))
      ->set_color({0.3f, 0.3f, 0.3f, 0.5f});
    create_style("dropdown:hover")->set_cursor(axgl::gui::Cursor::kPointer);
    create_style("dropdown:focus")->set_color({0.3f, 0.3f, 0.3f, 0.8f});
  }

  void register_page(
    const std::string& path, axgl::ptr_t<axgl::gui::Page> page
  ) override {
    if (!page) {
      unregister_page(path);
      return;
    }
    if (context_) page->set_context(context_);
    const auto normalized = normalize_path(path);
    pages_[normalized] = std::move(page);
    if (is_ancestor_path(normalized, active_path_)) resolve_active_page();
  }

  void unregister_page(const std::string& path) override {
    const auto normalized = normalize_path(path);
    pages_.erase(normalized);
    if (is_ancestor_path(normalized, active_path_)) resolve_active_page();
  }

  void navigate(const std::string& path) override {
    active_path_ = normalize_path(path);
    resolve_active_page();
  }

  [[nodiscard]] axgl::ptr_t<axgl::gui::Page> get_active_page() const override {
    return active_page_;
  }
  [[nodiscard]] const std::string& get_active_path() const override {
    return active_path_;
  }

  void terminate() override {
    pages_.clear();
    active_path_ = "/";
    active_page_ = nullptr;
    active_initialized_ = false;
  }

protected:
  static std::string normalize_path(const std::string& path) {
    std::string normalized;
    normalized.reserve(path.size() + 1);
    bool previous_slash = false;
    for (const char ch : path) {
      const bool is_slash = ch == '/';
      if (normalized.empty() && !is_slash) normalized.push_back('/');
      if (is_slash && previous_slash) continue;
      normalized.push_back(ch);
      previous_slash = is_slash;
    }
    if (normalized.empty()) return "/";
    while (normalized.size() > 1 && normalized.back() == '/')
      normalized.pop_back();
    return normalized;
  }

  [[nodiscard]] axgl::ptr_t<axgl::gui::Page> find_page(
    const std::string& path
  ) const {
    std::string candidate = path;
    while (!candidate.empty()) {
      if (const auto it = pages_.find(candidate); it != pages_.end())
        return it->second;

      if (candidate == "/") break;
      const auto slash = candidate.find_last_of('/');
      candidate.erase(slash == std::string::npos ? 0 : slash);
      if (candidate.empty()) candidate = "/";
    }
    return nullptr;
  }

  [[nodiscard]] static bool is_ancestor_path(
    const std::string& ancestor, const std::string& path
  ) {
    if (ancestor == "/" || ancestor == path) return true;
    return path.size() > ancestor.size() && path.starts_with(ancestor)
      && path[ancestor.size()] == '/';
  }

  void resolve_active_page() {
    auto page = find_page(active_path_);
#ifdef AXGL_DEBUG
    if (!page)
      AXGL_LOG_WARN("GuiService: no page registered for \"{}\".", active_path_);
#endif
    if (page == active_page_) return;
    active_page_ = std::move(page);
    active_initialized_ = false;
  }

  void update_active_page() {
    if (!active_page_) return;
    if (!active_initialized_) {
      active_page_->init();
      active_initialized_ = true;
    }
    AXGL_PROFILE_SCOPE("GUI Update");
    active_page_->update();
  }

  void set_element_default_style(
    const std::string& type_id, const axgl::ptr_t<axgl::gui::Element>& element
  ) {
    const auto split_result = util::split_string(type_id, ':');
    if (split_result.size() < 2 || split_result[0] != "element") return;

    const auto style = get_style(split_result[1]);
    if (!style) return;

    element->append_style(split_result[1]);
  }
};

} // namespace axgl::impl
