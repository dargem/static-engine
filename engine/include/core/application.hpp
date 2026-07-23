#pragma once

#include "core/logger.hpp"
#include "defines.hpp"
#include "game_types.hpp"
#include "platform/wayland.hpp"
#include <expected>

namespace static_eng {

class Application {
public:
  Application(Game&, Logger&);

  // Wrapper that returns a std::expected when constructing object
  static auto make_application(Game&, Logger&)
      -> std::expected<Application, std::string>;

  auto run() -> b8;

  auto get_game_inst() -> Game&;

private:
  Logger logger;
  platform::WaylandWindow platform;
  Game& game_inst;
  b8 is_running;
  b8 is_suspended;
};

} // namespace static_eng