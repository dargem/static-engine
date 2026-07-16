#pragma once

#include "core/comp_string.hpp"
#include <algorithm>
#include <array>
#include <ranges>
#include <string_view>
#include <vector>

namespace core {

namespace detail {
// Load in raw character data, argument template type deduction has some issues
constexpr std::array RAW_KV_CONFIG = std::to_array<char>({
#embed CONFIGS_FILE_PATH
    , '\0'});

constexpr std::string_view VIEW_KV_CONFIGS{RAW_KV_CONFIG.data(),
                                           RAW_KV_CONFIG.size() - 1};

struct KeyValueStringPair {
  std::string_view key;
  std::string_view value;
};

} // namespace detail

/**
 * @brief Split's a string into parts. Delimiter's at the start will result in a
 * split still with an empty string_view at the start. 2 consecutive delimiters
 * will result in a empty string view between them. The test has examples.
 *
 * @tparam CS is the string literal to split
 * @tparam DELIMITER is the character it splits at
 * @return std::vector<std::string_view>
 */
consteval auto split(std::string_view s, char delimiter)
    -> std::vector<std::string_view> {
  std::vector<std::string_view> separated;
  size_t last_split{};
  for (auto [index, letter] : std::views::enumerate(s)) {
    if (letter == delimiter) {
      separated.push_back(s.substr(last_split, index - last_split));
      last_split = index + 1; // To skip this next time
    } else if (index == s.size() - 1) {
      // Need to have + 1 as we don't exclude current
      separated.push_back(s.substr(last_split, index - last_split + 1));
    }
  }

  return separated;
}

constexpr size_t NUM_CONFIGS = std::count(detail::VIEW_KV_CONFIGS.begin(),
                                          detail::VIEW_KV_CONFIGS.end(), '\n') +
                               1;

constexpr auto KV_STRING_PAIRS =
    [] -> std::array<detail::KeyValueStringPair, NUM_CONFIGS> {
  std::array<detail::KeyValueStringPair, NUM_CONFIGS> configs;

  auto lines = split(detail::VIEW_KV_CONFIGS, '\n');
  for (auto [index, line] : std::views::enumerate(lines)) {
    auto pair = split(line, ':');
    if (pair.size() != 2) {
      throw "Invalid config pair";
    }
    configs[index] = detail::KeyValueStringPair(pair[0], pair[1]);
  }

  return configs;
}();

} // namespace core
