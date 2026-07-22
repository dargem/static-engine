#pragma once

#include "defines.hpp"
#include "logger.hpp"
#include "platform/wayland.hpp"
namespace static_eng {

class Application {
public:
  Application(i16 width, i16 height, std::string_view log_file,
              std::string_view app_name);

  auto application_run() -> b8;

private:
  Logger logger;
  platform::WaylandWindow platform;
  b8 is_running;
};

} // namespace static_eng