#pragma once

#include "core/application.hpp"

namespace static_eng {

/**
 * @brief Construct a new Application
 *
 * @param game The game this app uses
 * @param startup_logger The logger used for the engine's startup phase
 */
Application::Application(Game& game, Logger& startup_logger)
    : logger(game.config.log_file),
      platform(game.config.app_name, game.config.width, game.config.height),
      game_inst(game) {
  if (game_inst.initialize(&game_inst)) {
    startup_logger.log<static_eng::LogLevel::FATAL>(
        "Game failed to initialize");
    exit(1);
  }

  game_inst.on_resize(&game_inst, platform.get_width(), platform.get_height());
}

auto Application::make_application(Game& game, Logger& startup_logger)
    -> std::expected<Application, std::string> {
  try {
    return std::expected<Application, std::string>{std::in_place, game,
                                                   startup_logger};
  } catch (const std::exception& e) {
    return std::unexpected(e.what());
  }
}

auto Application::run() -> b8 {
  while (is_running) {
    if (!platform.pump_messages()) {
      platform.set_running(false);
    }

    if (!is_suspended) {
      if (!game_inst.update(&game_inst, 0.0f)) {
        logger.log<LogLevel::FATAL>("Game updated failed, shutting down");
        is_running = false;
        break;
      }

      if (!game_inst.render(&game_inst, 0.0f)) {
        logger.log<LogLevel::FATAL>("Game render failed, shutting down");
        is_running = false;
        break;
      }
    }
  }

  return true; // We have ran the app
}

} // namespace static_eng