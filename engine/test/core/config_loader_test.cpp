#include "core/config_loader.hpp"

#include "gtest/gtest.h"

TEST(CONFIG_LOADING, Split) {
  auto arr = core::split<"test0ing0stuff", '0'>();
  EXPECT_EQ(arr.size(), 3);
  EXPECT_EQ(arr[0], "test");
  EXPECT_EQ(arr[1], "ing");
  EXPECT_EQ(arr[2], "stuff");
}