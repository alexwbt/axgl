#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <stdexcept>
#include <string>

#include <axgl/interface/service.hpp>
#include <axgl/service_container.hpp>

namespace {

class FakeService : public axgl::Service {
  int initialize_calls_ = 0;
  int terminate_calls_ = 0;
  bool keep_alive_ = false;
  std::int32_t priority_ = 0;

public:
  static constexpr std::string_view kTypeId = "service::fake";

  void set_context(const axgl::Context*) override {}
  void set_keep_alive(bool value) { keep_alive_ = value; }
  void set_priority(std::int32_t value) { priority_ = value; }
  [[nodiscard]] int initialize_calls() const { return initialize_calls_; }
  [[nodiscard]] int terminate_calls() const { return terminate_calls_; }

  void initialize() override { ++initialize_calls_; }
  void terminate() override { ++terminate_calls_; }
  bool keep_alive() override { return keep_alive_; }
  [[nodiscard]] std::int32_t priority() const override { return priority_; }
};

class AnotherService : public axgl::Service {
public:
  static constexpr std::string_view kTypeId = "service::another";
  void set_context(const axgl::Context*) override {}
};

} // namespace

TEST_SUITE("axgl::ServiceContainer registration") {
  TEST_CASE("register and get by type id") {
    axgl::ServiceContainer sc;
    auto svc = axgl::create_ptr<FakeService>();
    sc.register_service(FakeService::kTypeId.data(), svc);

    CHECK(sc.has_service(FakeService::kTypeId.data()));
    CHECK(sc.has_service_type<FakeService>(FakeService::kTypeId.data()));
    CHECK(sc.get_service<FakeService>(FakeService::kTypeId.data()) == svc);
  }

  TEST_CASE("register_service_t constructs and stores") {
    axgl::ServiceContainer sc;
    auto svc = sc.register_service_t<FakeService>();

    CHECK(svc != nullptr);
    CHECK(sc.has_service(FakeService::kTypeId.data()));
    CHECK(sc.get_service_t<FakeService>() == svc);
  }

  TEST_CASE("duplicate registration throws in debug") {
    axgl::ServiceContainer sc;
    sc.register_service_t<FakeService>();
#ifdef AXGL_DEBUG
    CHECK_THROWS_AS(sc.register_service_t<FakeService>(), std::runtime_error);
#else
    CHECK_NOTHROW(sc.register_service_t<FakeService>());
#endif
  }

  TEST_CASE("remove_service removes it") {
    axgl::ServiceContainer sc;
    sc.register_service_t<FakeService>();
    sc.remove_service(FakeService::kTypeId.data());
    CHECK_FALSE(sc.has_service(FakeService::kTypeId.data()));
  }

  TEST_CASE("get_service for missing id throws in debug") {
    axgl::ServiceContainer sc;
#ifdef AXGL_DEBUG
    CHECK_THROWS_AS(
      (void)sc.get_service<FakeService>("service::missing"), std::runtime_error
    );
#else
    CHECK(sc.get_service<FakeService>("service::missing") == nullptr);
#endif
  }

  TEST_CASE("get_service with wrong type returns null / throws in debug") {
    axgl::ServiceContainer sc;
    sc.register_service_t<AnotherService>();
#ifdef AXGL_DEBUG
    CHECK_THROWS_AS(
      (void)sc.get_service<FakeService>(AnotherService::kTypeId.data()),
      std::runtime_error
    );
#else
    CHECK(
      sc.get_service<FakeService>(AnotherService::kTypeId.data()) == nullptr
    );
#endif
  }
}

TEST_SUITE("axgl::ServiceContainer lifecycle") {
  TEST_CASE("initialize / terminate call through to services") {
    axgl::ServiceContainer sc;
    auto svc = sc.register_service_t<FakeService>();
    sc.initialize();
    CHECK(svc->initialize_calls() == 1);
    sc.terminate();
    CHECK(svc->terminate_calls() == 1);
  }

  TEST_CASE("terminate runs in reverse order") {
    axgl::ServiceContainer sc;
    sc.register_service_t<FakeService>();
    sc.register_service_t<AnotherService>();
    CHECK_NOTHROW(sc.terminate());
  }
}

TEST_SUITE("axgl::ServiceContainer priority") {
  TEST_CASE("reorder_services sorts descending by priority") {
    axgl::ServiceContainer sc;
    auto low = axgl::create_ptr<FakeService>();
    low->set_priority(1);
    auto high = axgl::create_ptr<FakeService>();
    high->set_priority(100);
    sc.register_service("service::low", low);
    sc.register_service("service::high", high);

    sc.reorder_services();
    const auto services = sc.services();
    REQUIRE(services.size() == 2);
    CHECK(services[0] == high);
    CHECK(services[1] == low);
  }

  TEST_CASE("initialize respects reordered priority") {
    axgl::ServiceContainer sc;
    auto low = sc.register_service_t<FakeService>();
    low->set_priority(1);
    auto high = axgl::create_ptr<FakeService>();
    high->set_priority(100);
    sc.register_service("service::high", high);

    sc.initialize();
    const auto services = sc.services();
    REQUIRE(services.size() == 2);
    CHECK(services[0] == high);
    CHECK(services[1] == low);
  }
}

TEST_SUITE("axgl::ServiceContainer running") {
  TEST_CASE("running is true while any service keeps alive") {
    axgl::ServiceContainer sc;
    auto svc = sc.register_service_t<FakeService>();
    CHECK_FALSE(sc.running());
    svc->set_keep_alive(true);
    CHECK(sc.running());
    svc->set_keep_alive(false);
    CHECK_FALSE(sc.running());
  }
}

TEST_SUITE("axgl::ServiceContainer exec") {
  TEST_CASE("empty command is a no-op") {
    axgl::ServiceContainer sc;
    CHECK_NOTHROW(sc.exec(""));
  }
}
