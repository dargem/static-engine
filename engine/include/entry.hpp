#pragma once

#include "core/application.hpp"
#include "core/logger.hpp"
#include "game_types.hpp"

namespace static_eng {

// Externally defined function to create a game
extern auto create_game(Game& out_game) -> b8;

} // namespace static_eng

// Entry point of application
auto main() -> int;