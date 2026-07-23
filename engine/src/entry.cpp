#include "entry.hpp"
#include "logger.hpp"

auto main() -> int {
  // Specific file for logging info on engine initialization
  static_eng::Logger startup_logger("engine_init.txt");

  static_eng::Game game_inst;

  // Default configs
  game_inst.config = {
      .width = 720,
      .height = 1280,
      .log_file = "logs.txt",
      .app_name = "Static Engine",
  };

  if (!create_game(game_inst)) {
    startup_logger.log<static_eng::LogLevel::FATAL>("Game failed to create");
    return 1;
  }

  if (!game_inst.initialize || !game_inst.update || !game_inst.render ||
      !game_inst.on_resize) {
    startup_logger.log<static_eng::LogLevel::FATAL>(
        "Game does not have all its functions assigned");
    return 1;
  }

  static_eng::Application app(game_inst, startup_logger);

  app.run();
  return 0;
}