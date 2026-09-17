#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>
#include <vector>

#include <util/string.hpp>

using namespace util;

TEST_SUITE("util::split_string") {
  TEST_CASE("splits on delimiter") {
    const auto tokens = split_string("a,b,c", ',');
    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0] == "a");
    CHECK(tokens[1] == "b");
    CHECK(tokens[2] == "c");
  }

  TEST_CASE("single token without delimiter") {
    const auto tokens = split_string("only", ',');
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0] == "only");
  }

  TEST_CASE("empty string yields no tokens") {
    const auto tokens = split_string("", ',');
    CHECK(tokens.empty());
  }

  TEST_CASE("trailing delimiter yields no trailing empty token") {
    const auto tokens = split_string("a,b,", ',');
    REQUIRE(tokens.size() == 2);
    CHECK(tokens[0] == "a");
    CHECK(tokens[1] == "b");
  }

  TEST_CASE("output iterator overload") {
    std::vector<std::string> out;
    split_string("x|y|z", '|', std::back_inserter(out));
    REQUIRE(out.size() == 3);
    CHECK(out[2] == "z");
  }
}

TEST_SUITE("util::trim") {
  TEST_CASE("trim removes surrounding whitespace") {
    std::string s = "  hello  ";
    trim(s);
    CHECK(s == "hello");
  }

  TEST_CASE("trim_copy returns trimmed copy") {
    CHECK(trim_copy("  hi  ") == "hi");
    CHECK(trim_copy("nochange") == "nochange");
  }

  TEST_CASE("ltrim / rtrim / copies") {
    CHECK(ltrim_copy("  left") == "left");
    CHECK(rtrim_copy("right  ") == "right");
    CHECK(ltrim_copy("none") == "none");
    CHECK(rtrim_copy("none") == "none");
  }

  TEST_CASE("all-whitespace trims to empty") {
    CHECK(trim_copy("   \t\n  ") == "");
  }
}

TEST_SUITE("util::hash_string") {
  TEST_CASE("constexpr hash matches runtime") {
    constexpr auto ce = hash_string("hello");
    const auto rt = hash_string(std::string{"hello"});
    CHECK(ce == rt);
  }

  TEST_CASE("different strings differ") {
    CHECK(hash_string("a") != hash_string("b"));
  }

  TEST_CASE("empty string returns seed") {
    CHECK(hash_string("") == 5381);
  }
}

TEST_SUITE("util::string_to_vec3") {
  TEST_CASE("parses comma-separated values") {
    float x = 0, y = 0, z = 0;
    string_to_vec3("1.5,2.5,3.5", x, y, z);
    CHECK(x == doctest::Approx(1.5));
    CHECK(y == doctest::Approx(2.5));
    CHECK(z == doctest::Approx(3.5));
  }

  TEST_CASE("malformed input sets zero") {
    float x = 9, y = 9, z = 9;
    string_to_vec3("bad", x, y, z);
    CHECK(x == 0);
    CHECK(y == 0);
    CHECK(z == 0);
  }
}
