#include "core/config_loader.hpp"

#include "gtest/gtest.h"
#include <string_view>

TEST(CONFIG_LOADING, SanitizeString) {
  // By default should drop leading / trailing spaces
  EXPECT_EQ(core::sanitize("no_filter_needed"), "no_filter_needed");
  EXPECT_EQ(core::sanitize("no filter needed"), "no filter needed");
  EXPECT_EQ(core::sanitize("    lead_filter_needed"), "lead_filter_needed");
  EXPECT_EQ(core::sanitize("    trail_filter_needed"), "trail_filter_needed");
  EXPECT_EQ(core::sanitize("  spaces_each_side  "), "spaces_each_side");
  EXPECT_EQ(core::sanitize("  spaces between words  "), "spaces between words");
  EXPECT_EQ(core::sanitize("@custom@char@@", '@'), "custom@char");
}

TEST(CONFIG_LOADING, StringSplit) {

  // Sanitize is consteval and returns a vector so we need to test this in a
  // constantly evaluated context.
  auto endline_split = [] -> bool {
    auto result = core::split("line1\nline2\nline3", '\n');
    auto expected = {"line1", "line2", "line3"};
    return std::ranges::equal(result, expected);
  }();

  auto none_needed = [] -> bool {
    auto result = core::split("line1", '\n');
    auto expected = {"line1"};
    return std::ranges::equal(result, expected);
  }();

  auto consecutive_delimiters = [] -> bool {
    auto result = core::split("line1\n\nline2", '\n');
    auto expected = {"line1", "line2"};
    return std::ranges::equal(result, expected);
  }();

  auto start_end_delimiters = [] -> bool {
    auto result = core::split("\nline1\n", '\n');
    auto expected = {"line1"};
    return std::ranges::equal(result, expected);
  }();

  auto custom_delimiter = [] -> bool {
    auto result = core::split("a:b:c", ':');
    auto expected = {"a", "b", "c"};
    return std::ranges::equal(result, expected);
  }();

  EXPECT_TRUE(endline_split);
  EXPECT_TRUE(none_needed);
  EXPECT_TRUE(consecutive_delimiters);
  EXPECT_TRUE(start_end_delimiters);
  EXPECT_TRUE(custom_delimiter);
}