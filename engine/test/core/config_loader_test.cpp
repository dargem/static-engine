#include "core/config_loader.hpp"

#include "gtest/gtest.h"

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
// TEST(CONFIG_LOADING, StringSplit) {
//   auto arr = core::split<"test0ing0stuff", '0'>();
//   EXPECT_EQ(arr.size(), 3);
//   EXPECT_EQ(arr[0], "test");
//   EXPECT_EQ(arr[1], "ing");
//   EXPECT_EQ(arr[2], "stuff");

//   auto arr1 = core::split<"*example*string**", '*'>();
//   EXPECT_EQ(arr1.size(), 5);
//   EXPECT_EQ(arr1[0], "");
//   EXPECT_EQ(arr1[1], "example");
//   EXPECT_EQ(arr1[2], "string");
//   EXPECT_EQ(arr1[3], "");
//   EXPECT_EQ(arr1[4], "");
// }