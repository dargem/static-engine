#pragma once

#include <cstdint>

namespace static_eng {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

using b8 = bool;

// This will certainly catch plenty of bugs
static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);

static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);

static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

static_assert(sizeof(bool) == 1);

} // namespace static_eng