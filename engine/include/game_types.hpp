#pragma once

#include "core/application_config.hpp"
#include "defines.hpp"

namespace static_eng {

struct Game {
  // The apps config
  static_eng::ApplicationConfig config;

  // Function pointer to game's initialize function
  auto (*initialize)(struct Game* game_inst) -> static_eng::b8;

  // Function pointer to game's update function
  b8 (*update)(struct Game* game_inst, f32 delta_time);

  // Function pointer to game's render function
  b8 (*render)(struct Game* game_inst, f32 delta_time);

  // Function pointer to handle resizes
  void (*on_resize)(struct Game* game_inst, u32 width, u32 height);

  // Game specific state
  void* state;
};

} // namespace static_eng