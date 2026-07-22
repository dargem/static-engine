#pragma once

#include "utils/config_loader.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

namespace static_eng {

enum class LogLevel : uint8_t { TRACE = 0, DEBUG, INFO, WARN, ERROR, FATAL };

// A file logger class, will log everything as or more critical than
// keptLogLevel

namespace detail {
template <LogLevel KEPT_LOG_LEVEL> class BackingLogger {
public:
  BackingLogger(std::string_view filename) {
    const std::filesystem::path path{filename};

    // Check if file exists
    bool file_exists = std::filesystem::exists(path);
    std::ios_base::openmode mode = file_exists ? std::ios::app : std::ios::out;

    log_file.open(path, mode);

    if (!log_file.is_open()) {
      if (file_exists) {
        throw std::runtime_error(
            "File exists, but unable to open file for writing error logs");
      }
      throw std::runtime_error(
          "Created a file but unable to open file for writing error logs");
    }
  }

  BackingLogger(const BackingLogger&) = delete;
  auto operator=(BackingLogger&) -> BackingLogger& = delete;

  // Clean up holding the file
  ~BackingLogger() = default;

  // Log a message to the logger's file. This call should be completely
  // optimized away at compile time, if it doesn't log.
  template <LogLevel THIS_LOGS_LEVEL> void log(std::string_view message) {
    assert(log_file.is_open() && "File should always be open");

    using BackingType = std::underlying_type_t<LogLevel>;
    if constexpr (static_cast<BackingType>(THIS_LOGS_LEVEL) >=
                  static_cast<BackingType>(KEPT_LOG_LEVEL)) {
      // for C++ 20 this works and onwards as a static assert is only evaluated
      // if the constexpr expression its in is true. Prior to C++ 20 this would
      // fail at compile time surprisingly
      log_file << level_to_string(THIS_LOGS_LEVEL) << ": " << message << '\n';
      log_file.flush();
    }
  }

private:
  static consteval auto level_to_string(LogLevel level) -> std::string_view {
    switch (level) {
    case LogLevel::TRACE:
      return "TRACE";
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::FATAL:
      return "FATAL";
    }
    throw "Unknown log level";
    // Fallback, if adding another log level make sure to map it
    return "UNKNOWN";
  }
  std::ofstream log_file;
};

} // namespace detail

static constexpr LogLevel KEPT_LOG_LEVEL = [] -> LogLevel {
  constexpr auto LEVEL = utils::get_config<"log_level">();
  if (LEVEL == "TRACE")
    return LogLevel::TRACE;
  if (LEVEL == "DEBUG")
    return LogLevel::DEBUG;
  if (LEVEL == "INFO")
    return LogLevel::INFO;
  if (LEVEL == "WARN")
    return LogLevel::WARN;
  if (LEVEL == "ERROR")
    return LogLevel::ERROR;
  if (LEVEL == "FATAL")
    return LogLevel::FATAL;
  throw "Unknown log level";
}();

class Logger {
public:
  Logger(const Logger&) = delete;
  auto operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  auto operator=(Logger&&) = delete;

  /**
   * @brief Get a logger instance
   * This internally delegates to a backing logger which it returns
   * @return detail::BackingLogger<KEPT_LOG_LEVEL>&
   */
  static auto get_instance() -> detail::BackingLogger<KEPT_LOG_LEVEL>& {
    // Should update log path in the future
    static detail::BackingLogger<KEPT_LOG_LEVEL> logger("logs.txt");
    return logger;
  }

private:
  Logger() = default;
};

// using Logger = detail::BackingLogger<KEPT_LOG_LEVEL>;

} // namespace static_eng