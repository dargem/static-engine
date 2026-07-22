#include "platform/wayland.hpp"
#include <iostream>

using static_eng::platform::WaylandError;
using static_eng::platform::WaylandWindow;

auto main() -> int {
  try {
    WaylandWindow window("My Engine", 1280, 720);

    while (window.pump_messages()) {
      // engine tick / render here
    }
  } catch (const WaylandError& e) {
    std::cerr << "Wayland init failed: " << e.what() << '\n';
    return 1;
  }

  return 0;
}