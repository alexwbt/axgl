#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <limits>
#include <optional>

#include <util/numcast.hpp>

using namespace util;

TEST_SUITE("util::in_range") {
  TEST_CASE("same type is always in range") {
    CHECK(in_range<int>(42));
    CHECK(in_range<double>(3.14));
    CHECK(in_range<int>(std::numeric_limits<int>::min()));
    CHECK(in_range<int>(std::numeric_limits<int>::max()));
  }

  TEST_CASE("integral -> integral, same signedness") {
    CHECK(in_range<int8_t>(int8_t{127}));
    CHECK_FALSE(in_range<int8_t>(int{128}));
    CHECK(in_range<int8_t>(int{127}));
    CHECK(in_range<uint8_t>(static_cast<unsigned>(255)));
    CHECK_FALSE(in_range<uint8_t>(static_cast<unsigned>(256)));
  }

  TEST_CASE("signed -> unsigned rejects negatives") {
    CHECK_FALSE(in_range<uint8_t>(-1));
    CHECK(in_range<uint8_t>(0));
    CHECK(in_range<uint8_t>(255));
    CHECK_FALSE(in_range<uint8_t>(256));
  }

  TEST_CASE("unsigned -> signed") {
    CHECK(in_range<int8_t>(uint8_t{0}));
    CHECK(in_range<int8_t>(uint8_t{127}));
    CHECK_FALSE(in_range<int8_t>(uint8_t{128}));
  }

  TEST_CASE("integral -> float roundtrips") {
    CHECK(in_range<double>(int64_t{1} << 53));
    CHECK_FALSE(in_range<double>(std::numeric_limits<int64_t>::max()));
    CHECK(in_range<float>(0));
  }

  TEST_CASE("float -> float precision") {
    CHECK(in_range<float>(0.0));
    CHECK(in_range<float>(1.0));
  }

  TEST_CASE("float -> integral rejects non-finite at runtime") {
    CHECK_FALSE(in_range<int>(std::numeric_limits<float>::infinity()));
    CHECK_FALSE(in_range<int>(-std::numeric_limits<float>::infinity()));
    CHECK_FALSE(in_range<int>(std::numeric_limits<float>::quiet_NaN()));
  }

  TEST_CASE("float -> integral bounds") {
    CHECK(in_range<int>(0.0f));
    const float big
      = static_cast<float>(std::numeric_limits<int>::max()) * 2.0f;
    CHECK_FALSE(in_range<int>(big));
  }
}

TEST_SUITE("util::narrow_cast") {
  TEST_CASE("returns optional on success") {
    const auto v = narrow_cast<int8_t>(int{100});
    CHECK(v.has_value());
    CHECK(*v == 100);
  }

  TEST_CASE("returns nullopt on overflow") {
    const auto v = narrow_cast<int8_t>(int{200});
    CHECK_FALSE(v.has_value());
  }

  TEST_CASE("fallback overload returns fallback on overflow") {
    CHECK(narrow_cast(int{200}, int8_t{-1}) == int8_t{-1});
    CHECK(narrow_cast(int{100}, int8_t{-1}) == int8_t{100});
  }
}

TEST_SUITE("util::narrow") {
  TEST_CASE("throws on out of range") {
    CHECK_THROWS_AS((void)narrow<int8_t>(int{200}), std::out_of_range);
    CHECK(narrow<int8_t>(int{100}) == 100);
  }
}

TEST_SUITE("util::clamp_cast") {
  TEST_CASE("integral -> integral, same signedness, saturates") {
    CHECK(clamp_cast<int8_t>(int{200}) == int8_t{127});
    CHECK(clamp_cast<int8_t>(int{-200}) == int8_t{-128});
    CHECK(clamp_cast<int8_t>(int{50}) == int8_t{50});
  }

  TEST_CASE("signed -> unsigned negatives clamp to 0") {
    CHECK(clamp_cast<uint8_t>(-1) == uint8_t{0});
    CHECK(clamp_cast<uint8_t>(300) == uint8_t{255});
    CHECK(clamp_cast<uint8_t>(100) == uint8_t{100});
  }

  TEST_CASE("unsigned -> signed saturates at max") {
    CHECK(clamp_cast<int8_t>(uint8_t{200}) == int8_t{127});
    CHECK(clamp_cast<int8_t>(uint8_t{50}) == int8_t{50});
  }

  TEST_CASE("float -> integral maps non-finite to 0") {
    CHECK(clamp_cast<int>(std::numeric_limits<float>::infinity()) == 0);
    CHECK(clamp_cast<int>(-std::numeric_limits<float>::infinity()) == 0);
    CHECK(clamp_cast<int>(std::numeric_limits<float>::quiet_NaN()) == 0);
  }

  TEST_CASE("float -> integral saturates at bounds") {
    CHECK(clamp_cast<int8_t>(1e10f) == int8_t{127});
    CHECK(clamp_cast<int8_t>(-1e10f) == int8_t{-128});
    CHECK(clamp_cast<int8_t>(5.0f) == int8_t{5});
  }

  TEST_CASE("integral -> float is identity") {
    CHECK(clamp_cast<float>(int{42}) == 42.0f);
  }

  TEST_CASE("float -> float clamps to range") {
    const double over
      = static_cast<double>(std::numeric_limits<float>::max()) * 2.0;
    CHECK(clamp_cast<float>(over) == std::numeric_limits<float>::max());
  }
}
