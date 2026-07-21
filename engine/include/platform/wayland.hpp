// RAII wrapper around a Wayland client connection + xdg-shell toplevel
// window

#pragma once

// #ifdef HAVE_WAYLAND

#include "defines.hpp"
#include "platform/xdg-shell-client-protocol.h"
#include <wayland-client.h>

#include <optional>
#include <stdexcept>
#include <string>

namespace static_eng::platform {
class WaylandError : public std::runtime_error {
public:
  explicit WaylandError(const std::string& what) : std::runtime_error(what) {}
};

// Small RAII wrapper around the shm-backed pixel buffer used to back
// the surface.
class ShmBuffer {
public:
  ShmBuffer(wl_shm* shm, i32 width, i32 height, u32 argb_colour);
  ~ShmBuffer();

  ShmBuffer(const ShmBuffer&) = delete;
  auto operator=(const ShmBuffer&) = delete;

  ShmBuffer(ShmBuffer&& other) noexcept;
  auto operator=(ShmBuffer&& other) noexcept -> ShmBuffer&;

  [[nodiscard]]
  auto handle() const -> wl_buffer* {
    return buffer;
  }

private:
  void release();

  wl_buffer* buffer = nullptr;
};

class WaylandWindow {
public:
  // Throws WaylandError on failure
  WaylandWindow(const std::string& application_name, i32 width, i32 height);
  ~WaylandWindow();

  WaylandWindow(const WaylandWindow&) = delete;
  auto operator=(const WaylandWindow&) = delete;

  WaylandWindow(WaylandWindow&&) = delete;
  auto operator=(WaylandWindow&&) = delete;

  // Returns false once the window should close (compositor sent
  // xdg_toplevel::close, or a dispatch error occurred).
  auto pump_messages() -> b8;

  [[nodiscard]]
  auto is_running() const -> b8 {
    return running;
  }

  [[nodiscard]]
  auto get_width() const -> i32 {
    return width;
  }

  [[nodiscard]]
  auto get_height() const -> i32 {
    return height;
  }

private:
  // static trampolines bridging the C callback ABI back to `this`
  static void on_registry_global(void* data, wl_registry* registry, u32 name,
                                 const char* interface, u32 version);
  static void on_registry_global_remove(void* data, wl_registry* registry,
                                        u32 name);

  static void on_wm_base_ping(void* data, xdg_wm_base* wm_base, u32 serial);

  static void on_xdg_surface_configure(void* data, xdg_surface* surface,
                                       u32 serial);

  static void on_xdg_toplevel_configure(void* data, xdg_toplevel* toplevel,
                                        i32 width, i32 height,
                                        wl_array* states);
  static void on_xdg_toplevel_close(void* data, xdg_toplevel* toplevel);

  // setup helpers, called from the constructor
  void connect_display();
  void bind_globals();
  void create_window(const std::string& application_name);
  void initial_commit();

  // Wayland object handles. Parents before children so that children get
  // destroyed before their parents.
  wl_display* display = nullptr;
  wl_registry* registry = nullptr;
  wl_compositor* compositor = nullptr;
  wl_shm* shm = nullptr;
  wl_seat* seat = nullptr;
  xdg_wm_base* wm_base = nullptr;

  wl_surface* surface = nullptr;
  xdg_surface* xdg_surface = nullptr;
  xdg_toplevel* xdg_toplevel = nullptr;

  // Declared after so its destroyed first. ShmBuffer uses RAII.
  std::optional<ShmBuffer> buffer;

  i32 width = 0;
  i32 height = 0;
  bool running = true;
  bool surface_configured = false;

  static const wl_registry_listener REGISTRY_LISTENER;
  static const xdg_wm_base_listener WM_BASE_LISTENER;
  static const xdg_surface_listener XDG_SURFACE_LISTENER;
  static const xdg_toplevel_listener XDG_TOPLEVEL_LISTENER;
};

} // namespace static_eng::platform

// #endif