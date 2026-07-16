#pragma once

#include <algorithm>
#include <array>
#include <string_view>

namespace core {

template <size_t N> struct CompString {
  constexpr CompString(const char (&arr)[N]) {
    std::copy_n(arr, N, data.data());
  }
  consteval auto view() const { return std::string_view(data.data(), N - 1); }
  std::array<char, N> data;
};

} // namespace core