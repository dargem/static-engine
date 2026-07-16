#pragma once

#include "core/comp_string.hpp"
#include <algorithm>
#include <array>
#include <ranges>
#include <string_view>

namespace core {

namespace detail {
// Load in raw character data, argument template type deduction has some issues
constexpr std::array RAW_CONFIG = std::to_array<char>({
#embed CONFIGS_FILE_PATH
    , '\0'});

constexpr std::string_view CHAR_KV_CONFIGS{RAW_CONFIG.data(),
                                           RAW_CONFIG.size() - 1};

struct KeyValueStringPair {
  std::string_view key;
  std::string_view value;
};

} // namespace detail

template <CompString CS, char DELIMITER> consteval auto split() -> auto {
  constexpr auto S = CS.view();
  constexpr size_t N = std::count(S.begin(), S.end(), DELIMITER);

  std::array<std::string_view, N + 1> separated;
  size_t last_split{};
  size_t num_splits{};
  for (auto [index, letter] : std::views::enumerate(S)) {
    if (letter == DELIMITER) {
      separated[num_splits] = S.substr(last_split, index - last_split);
      last_split = index + 1; // To skip this next time
      ++num_splits;
    } else if (index == S.size() - 1) {
      // Need to have +1 as we don't exclude current
      separated[num_splits] = S.substr(last_split, index - last_split + 1);
    }
  }

  return separated;
}
// constexpr size_t NUM_PAIRS = [] -> size_t {

// }();

} // namespace core
