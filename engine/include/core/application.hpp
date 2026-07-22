#pragma once

#include "core/logger.hpp"
#include "defines.hpp"
#include "game_types.hpp"
#include "platform/wayland.hpp"

namespace static_eng {

class Application {
public:
  Application(Game&);

  auto run() -> b8;

private:
  Logger logger;
  platform::WaylandWindow platform;
  Game& game_inst;
  b8 is_running;
  b8 is_suspended;
};

} // namespace static_eng