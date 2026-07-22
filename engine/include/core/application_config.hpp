#pragma once

#include "defines.hpp"
#include <string_view>

namespace static_eng {

struct ApplicationConfig {
  i16 width;
  i16 height;
  std::string_view log_file;
  std::string_view app_name;
};

} // namespace static_eng