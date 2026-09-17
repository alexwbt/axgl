#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <opengl/text.hpp>

using opengl::WrapMode;
using namespace opengl::text_detail;

TEST_SUITE("opengl::text_detail::is_cjk_char") {
  TEST_CASE("common CJK ideographs are CJK") {
    CHECK(is_cjk_char(U'一'));
    CHECK(is_cjk_char(U'龿'));
    CHECK(is_cjk_char(U'蒙'));
  }

  TEST_CASE("CJK extension A is CJK") {
    CHECK(is_cjk_char(U'㐀'));
    CHECK(is_cjk_char(U'䶟'));
  }

  TEST_CASE("hiragana and katakana are CJK") {
    CHECK(is_cjk_char(U'あ'));
    CHECK(is_cjk_char(U'カ'));
  }

  TEST_CASE("hangul syllables are CJK") {
    CHECK(is_cjk_char(U'가'));
    CHECK(is_cjk_char(U'힣'));
  }

  TEST_CASE("ASCII is not CJK") {
    CHECK_FALSE(is_cjk_char(U'A'));
    CHECK_FALSE(is_cjk_char(U'z'));
    CHECK_FALSE(is_cjk_char(U' '));
  }

  TEST_CASE("Latin-1 supplement is not CJK") {
    CHECK_FALSE(is_cjk_char(U'é'));
    CHECK_FALSE(is_cjk_char(U'ü'));
  }
}

TEST_SUITE("opengl::text_detail::is_word_boundary_char") {
  TEST_CASE("whitespace is a word boundary") {
    CHECK(is_word_boundary_char(U' '));
    CHECK(is_word_boundary_char(U'\t'));
    CHECK(is_word_boundary_char(U'\n'));
    CHECK(is_word_boundary_char(U'\r'));
  }

  TEST_CASE("non-whitespace is not a word boundary") {
    CHECK_FALSE(is_word_boundary_char(U'A'));
    CHECK_FALSE(is_word_boundary_char(U'一'));
  }
}

TEST_SUITE("opengl::text_detail::is_no_break_before_char") {
  TEST_CASE("combining marks are no-break-before") {
    CHECK(is_no_break_before_char(U'\u0301'));
    CHECK(is_no_break_before_char(U'\u0300'));
  }

  TEST_CASE("zero-width joiner is no-break-before") {
    CHECK(is_no_break_before_char(U'\u200D'));
  }

  TEST_CASE("closing punctuation is no-break-before") {
    CHECK(is_no_break_before_char(U')'));
    CHECK(is_no_break_before_char(U']'));
    CHECK(is_no_break_before_char(U'}'));
  }

  TEST_CASE("CJK closing brackets are no-break-before") {
    CHECK(is_no_break_before_char(U'】'));
    CHECK(is_no_break_before_char(U'〕'));
    CHECK(is_no_break_before_char(U'］'));
  }

  TEST_CASE("regular letters are not no-break-before") {
    CHECK_FALSE(is_no_break_before_char(U'A'));
    CHECK_FALSE(is_no_break_before_char(U'一'));
  }
}

TEST_SUITE("opengl::text_detail::can_break_before") {
  TEST_CASE("None mode never breaks") {
    CHECK_FALSE(can_break_before(U'A', U'B', WrapMode::None));
    CHECK_FALSE(can_break_before(U'一', U'丁', WrapMode::None));
  }

  TEST_CASE("Char mode always breaks (except handled elsewhere)") {
    CHECK(can_break_before(U'A', U'B', WrapMode::Char));
    CHECK(can_break_before(U'一', U'丁', WrapMode::Char));
  }

  TEST_CASE("Word mode breaks at whitespace") {
    CHECK(can_break_before(U' ', U'A', WrapMode::Word));
    CHECK(can_break_before(U'A', U' ', WrapMode::Word));
  }

  TEST_CASE("Word mode does not break between Latin letters") {
    CHECK_FALSE(can_break_before(U'A', U'B', WrapMode::Word));
    CHECK_FALSE(can_break_before(U'o', U'o', WrapMode::Word));
  }

  TEST_CASE("Auto mode breaks between CJK ideographs") {
    CHECK(can_break_before(U'一', U'二', WrapMode::Auto));
    CHECK(can_break_before(U'蒙', U'古', WrapMode::Auto));
  }

  TEST_CASE("Auto mode breaks between CJK and Latin") {
    CHECK(can_break_before(U'一', U'A', WrapMode::Auto));
    CHECK(can_break_before(U'A', U'一', WrapMode::Auto));
  }

  TEST_CASE("Auto mode does not break before combining mark") {
    CHECK_FALSE(can_break_before(U'a', U'\u0301', WrapMode::Auto));
    CHECK_FALSE(can_break_before(U'a', U'\u0301', WrapMode::Word));
  }

  TEST_CASE("Auto mode does not break before closing punctuation") {
    CHECK_FALSE(can_break_before(U'a', U')', WrapMode::Auto));
    CHECK_FALSE(can_break_before(U'一', U'】', WrapMode::Auto));
  }

  TEST_CASE("Auto mode breaks after whitespace even before CJK") {
    CHECK(can_break_before(U' ', U'一', WrapMode::Auto));
  }

  TEST_CASE(
    "Word mode does not break between Latin letters even if one is CJK-adjacent"
  ) {
    CHECK_FALSE(can_break_before(U'a', U'b', WrapMode::Word));
  }
}
