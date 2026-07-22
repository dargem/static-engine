#pragma once

#include "defines.hpp"
#include "platform/platform.hpp"
#include "platform/wayland.hpp"
namespace static_eng {

class Application {
public:
  Application(i16 width, i16 height, std::string_view log_file,
              std::string_view app_name)
      : logger(log_file), platform(app_name, width, height) {}

  auto application_run() -> b8 {
    while (is_running) {
      if (!platform.pump_messages()) {
        platform.set_running(false);
      }
    }

    is_running = false;
  }

private:
  Logger logger;
  platform::WaylandWindow platform;
  b8 is_running;
};

} // namespace static_eng