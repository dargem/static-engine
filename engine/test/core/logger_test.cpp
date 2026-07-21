#include "logger.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#include "gtest/gtest.h"

using static_eng::Logger;
using static_eng::LogLevel;
using static_eng::detail::BackingLogger;

namespace {
// location of written out file
constexpr std::string_view TEST_LOC{"test_logs.txt"};
constexpr std::string_view EXAMPLE_TXT{"An example log entry"};
constexpr std::string_view EXAMPLE_TXT_2{"A different log entry"};
} // namespace

TEST(BackingLogger, LoggersCanBeCreated) {
  EXPECT_NO_THROW(BackingLogger<LogLevel::INFO>{TEST_LOC})
      << "Should be able to build this";
  EXPECT_NO_THROW(BackingLogger<LogLevel::WARN>{TEST_LOC})
      << "Should be able to build this";
}

TEST(BackingLogger, LogsIntoFile) {
  // open and clear the file
  const std::filesystem::path path{TEST_LOC};
  // opens for the file for writing and truncate file length 0 (empties it)
  std::fstream log_file(path, std::ofstream::out | std::ofstream::trunc);
  EXPECT_TRUE(log_file.is_open()) << "File should be opened";
  log_file.close();

  // Log to the file
  std::optional<BackingLogger<LogLevel::INFO>> logger{TEST_LOC};
  logger.value().log<LogLevel::INFO>(EXAMPLE_TXT);
  // safely destroy the old logger
  logger.reset();

  log_file.open(path, std::ios::in);
  EXPECT_TRUE(log_file.is_open()) << "File should be opened";

  std::string line;
  std::getline(log_file, line);

  EXPECT_FALSE(line.empty()) << "Logged line should not be empty!";
  EXPECT_TRUE(line.contains(EXAMPLE_TXT))
      << "The logged file should contain the message it outputted";
  EXPECT_TRUE(line.contains("INFO"))
      << "Should contain the log level of the message";
}

TEST(BackingLogger, LogsOnlyHigherLevelWarnings) {
  // open and clear the file
  const std::filesystem::path path{TEST_LOC};
  // opens for the file for writing and truncate file length 0 (empties it)
  std::fstream log_file(path, std::ofstream::out | std::ofstream::trunc);
  EXPECT_TRUE(log_file.is_open()) << "File should be opened";
  log_file.close();

  // Log to the file
  std::optional<BackingLogger<LogLevel::WARN>> logger{TEST_LOC};
  logger.value().log<LogLevel::INFO>(EXAMPLE_TXT);
  logger.value().log<LogLevel::FATAL>(EXAMPLE_TXT_2);

  // safely destroy the old logger
  logger.reset();

  log_file.open(path, std::ios::in);
  EXPECT_TRUE(log_file.is_open()) << "File should be opened";

  std::string line;
  std::getline(log_file, line);

  EXPECT_FALSE(line.empty()) << "Logged line should not be empty!";
  EXPECT_FALSE(line.contains("INFO"))
      << "Should not log the INFO level message";
  EXPECT_FALSE(line.contains(EXAMPLE_TXT))
      << "Should only log WARN level and higher";
  EXPECT_TRUE(line.contains("FATAL"))
      << "Should log the Fatal level log message";
  EXPECT_TRUE(line.contains(EXAMPLE_TXT_2))
      << "Should contain the fatal level log message";
}