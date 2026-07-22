#pragma once

#include "application.hpp"

namespace static_eng {

Application::Application(i16 width, i16 height, std::string_view log_file,
                         std::string_view app_name)
    : logger(log_file), platform(app_name, width, height) {}

auto Application::application_run() -> b8 {
  while (is_running) {
    if (!platform.pump_messages()) {
      platform.set_running(false);
    }
  }

  return true; // We have ran the app
}

} // namespace static_eng