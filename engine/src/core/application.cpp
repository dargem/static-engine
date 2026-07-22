#pragma once

#include "core/application.hpp"

namespace static_eng {

Application::Application(Game& game)
    : logger(game.config.log_file),
      platform(game.config.app_name, game.config.width, game.config.height),
      game_inst(game) {}

auto Application::run() -> b8 {
  while (is_running) {
    if (!platform.pump_messages()) {
      platform.set_running(false);
    }
  }

  return true; // We have ran the app
}

} // namespace static_eng