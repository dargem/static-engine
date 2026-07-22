#include "application.hpp"
#include "platform/wayland.hpp"
#include <defines.hpp>

using static_eng::platform::PlatformError;
using static_eng::platform::WaylandWindow;

auto main() -> int {

  static_eng::i16 width{1280};
  static_eng::i16 height{960};
  std::string_view log_file{"logs.txt"};
  std::string_view app_name{"static_eng"};

  static_eng::Application app(width, height, log_file, app_name);
  app.application_run();

  return 0;
}