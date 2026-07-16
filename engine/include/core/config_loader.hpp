#pragma once

#include "core/comp_string.hpp"
#include <array>
#include <numeric>
#include <ranges>
#include <string_view>

namespace core {

namespace detail {
// Load in raw character data, argument template type deduction has some issues
constexpr std::array RAW_CONFIG = std::to_array<char>({
#embed "resources/configs.txt"
    , '\0'});

constexpr std::string_view CHAR_KV_CONFIGS{RAW_CONFIG.data(),
                                           RAW_CONFIG.size() - 1};

struct KeyValueStringPair {
  std::string_view key;
  std::string_view value;
};

} // namespace detail

consteval auto count_instances(std::string_view s, char target) -> size_t {
  return std::accumulate(s.begin(), s.end(), 0uz,
                         [target](size_t count, char letter) -> size_t {
                           return count + (letter == target);
                         });
}

template <CompString CS> consteval auto split(char delimiter) -> auto {
  constexpr auto S = CS.view();
  constexpr size_t N = count_instances(S, delimiter);

  std::array<std::string_view, N> separated;
  size_t last_split{};
  size_t num_splits{};
  for (auto [index, letter] : std::views::enumerate(S)) {
    if (letter != delimiter || index == (S.size() - 1)) {
      separated[num_splits] = S.substr(last_split, index - last_split);
      last_split = index + 1; // To skip this next time
      ++num_splits;
    }
  }

  return separated;
}
// constexpr size_t NUM_PAIRS = [] -> size_t {

// }();

} // namespace core
