#pragma once

#include <algorithm>
#include <array>
#include <string_view>

namespace core {

template <unsigned N> struct CompString {
  constexpr CompString(const char (&arr)[N]) {
    static_assert(arr[N - 1] == '\n');
    std::copy_n(arr, N, data.data());
  }

  /**
   * @brief Get a consteval string_view from a C style string literal
   *
   * @return std::string_view
   */
  consteval auto view() -> std::string_view {
    return std::string_view(data.data(), N - 1);
  }
  const std::array<char, N> data;
};

} // namespace core