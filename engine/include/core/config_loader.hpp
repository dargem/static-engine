#pragma once

#include "core/comp_string.hpp"
#include <algorithm>
#include <array>
#include <ranges>
#include <string_view>
#include <vector>

namespace core {

namespace detail {
// Load in raw character data, using a raw C-style array to avoid compiler bugs
// with std::to_array and #embed
constexpr char RAW_KV_CONFIG[] = {
#embed CONFIGS_FILE_PATH
    , '\0'};

constexpr std::string_view VIEW_KV_CONFIGS{RAW_KV_CONFIG,
                                           sizeof(RAW_KV_CONFIG) - 1};

struct KeyValueStringPair {
  std::string_view key;
  std::string_view value;
};

} // namespace detail

/**
 * @brief Split's a string into parts. A split which results in an empty
 * string_view will skip adding that view.
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
      if (index - last_split != 0) {
        separated.push_back(s.substr(last_split, index - last_split));
      }
      last_split = index + 1; // To skip this next time

    } else if (index + 1 == s.size()) {
      // Need to have + 1 as we don't exclude current
      if (index - last_split + 1 != 0) {
        separated.push_back(s.substr(last_split, index - last_split + 1));
      }
    }
  }

  return separated;
}

/**
 * @brief Sanitizes a inputted string_view by removing leading/trailing
 * characters which are equal to discarded
 *
 * @param s The string_view to sanitize
 * @param discarded
 * @return std::string_view of sanitized string
 */
consteval auto sanitize(std::string_view s, char discarded = ' ')
    -> std::string_view {
  size_t start{};
  size_t end{s.size()};

  for (; start < end && s[start] == discarded; ++start);
  for (; start < end && s[end - 1] == discarded; --end);

  return s.substr(start, end - start);
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
    configs[index] =
        detail::KeyValueStringPair(sanitize(pair[0]), sanitize(pair[1]));
  }

  return configs;
}();

/**
 * @brief Get a view of the
 *
 * @tparam S
 * @return std::string_view
 */
template <CompString S> constexpr auto get_config() -> std::string_view {
  for (auto [key, value] : KV_STRING_PAIRS) {
    if (key == S.view())
      return sanitize(value);
  }
  throw "Key not found";
}

} // namespace core
