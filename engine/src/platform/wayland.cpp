// #ifdef HAVE_WAYLAND

#include "platform/wayland.hpp"

#include <cstring>
#include <poll.h>
#include <sys/mman.h>
#include <unistd.h>

namespace static_eng::platform {

ShmBuffer::ShmBuffer(wl_shm* shm, i32 width, i32 height, u32 argb_colour) {
  const i32 stride = width * 4;
  const i32 size = stride * height;

  char name[] = "wl_shm_cpp";
  int fd = memfd_create(name, MFD_CLOEXEC);
  if (fd < 0) {
    throw WaylandError("Failed to create shm-backed temp file for buffer.");
  }
  shm_unlink(name);

  if (ftruncate(fd, size) < 0) {
    close(fd);
    throw WaylandError("ftruncate failed while sizing shm buffer.");
  }

  void* data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    close(fd);
    throw WaylandError("mmap failed while creating shm buffer.");
  }

  u32* pixels = static_cast<u32*>(data);
  for (i32 i = 0; i < width * height; ++i) {
    pixels[i] = argb_colour;
  }
  munmap(data, size);

  wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
  // The fd is only needed to create the pool; the pool retains its
  // own reference, so we can close our copy immediately afterwards.
  close(fd);

  buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride,
                                     WL_SHM_FORMAT_ARGB8888);
  wl_shm_pool_destroy(pool);

  if (!buffer) {
    throw WaylandError("Failed to create wl_buffer from shm pool.");
  }
}

ShmBuffer::~ShmBuffer() { release(); }

void ShmBuffer::release() {
  if (buffer) {
    wl_buffer_destroy(buffer);
    buffer = nullptr;
  }
}

ShmBuffer::ShmBuffer(ShmBuffer&& other) noexcept : buffer(other.buffer) {
  other.buffer = nullptr;
}

auto ShmBuffer::operator=(ShmBuffer&& other) noexcept -> ShmBuffer& {
  if (this != &other) {
    release();
    buffer = other.buffer;
    other.buffer = nullptr;
  }
  return *this;
}

// Wayland's add_listener functions take plain function pointers + a void* user
// data. There's no way to have them bind to `this` directly. So callback is a
// static member that casts data back to a WaylandWindow* that forwards to non
// static logic.

const wl_registry_listener WaylandWindow::REGISTRY_LISTENER = {
    .global = &WaylandWindow::on_registry_global,
    .global_remove = &WaylandWindow::on_registry_global_remove,
};

const xdg_wm_base_listener WaylandWindow::WM_BASE_LISTENER = {
    .ping = &WaylandWindow::on_wm_base_ping,
};

const xdg_surface_listener WaylandWindow::XDG_SURFACE_LISTENER = {
    .configure = &WaylandWindow::on_xdg_surface_configure,
};

const xdg_toplevel_listener WaylandWindow::XDG_TOPLEVEL_LISTENER = {
    .configure = &WaylandWindow::on_xdg_toplevel_configure,
    .close = &WaylandWindow::on_xdg_toplevel_close,
};

void WaylandWindow::on_registry_global(void* data, wl_registry* registry,
                                       u32 name, const char* interface,
                                       u32 version) {
  auto* self = static_cast<WaylandWindow*>(data);

  if (std::strcmp(interface, wl_compositor_interface.name) == 0) {
    self->compositor_ = static_cast<wl_compositor*>(
        wl_registry_bind(registry, name, &wl_compositor_interface, 4));
  } else if (std::strcmp(interface, wl_shm_interface.name) == 0) {
    self->shm_ = static_cast<wl_shm*>(
        wl_registry_bind(registry, name, &wl_shm_interface, 1));
  } else if (std::strcmp(interface, wl_seat_interface.name) == 0) {
    self->seat_ = static_cast<wl_seat*>(
        wl_registry_bind(registry, name, &wl_seat_interface, 5));
  } else if (std::strcmp(interface, xdg_wm_base_interface.name) == 0) {
    self->wm_base_ = static_cast<xdg_wm_base*>(
        wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
  }
}

void WaylandWindow::on_registry_global_remove(void*, wl_registry*, u32) {
  // A global went away (monitor unplugged, etc). Potentially add handling
  // later.
}

void WaylandWindow::on_wm_base_ping(void* data, xdg_wm_base* wm_base,
                                    u32 serial) {
  xdg_wm_base_pong(wm_base, serial);
}

void WaylandWindow::on_xdg_surface_configure(void* data,
                                             struct xdg_surface* surface,
                                             u32 serial) {
  auto* self = static_cast<WaylandWindow*>(data);
  xdg_surface_ack_configure(surface, serial);
  self->surface_configured = true;
}

void WaylandWindow::on_xdg_toplevel_configure(void* data, struct xdg_toplevel*,
                                              i32 width, i32 height,
                                              wl_array*) {
  auto* self = static_cast<WaylandWindow*>(data);
  // 0 means "compositor has no opinion, you decide" - keep current size.
  if (width > 0 && height > 0) {
    self->width = width;
    self->height = height;
  }
}

void WaylandWindow::on_xdg_toplevel_close(void* data, struct xdg_toplevel*) {
  auto* self = static_cast<WaylandWindow*>(data);
  self->running = false;
}

// Construction + Teardown
WaylandWindow::WaylandWindow(const std::string& application_name, i32 width,
                             i32 height)
    : width(width), height(height) {
  connect_display();
  bind_globals();
  create_window(application_name);
  initial_commit();
}

void WaylandWindow::connect_display() {
  display_ = wl_display_connect(nullptr);
  if (!display_) {
    throw WaylandError("Failed to connect to Wayland display.");
  }
}

void WaylandWindow::bind_globals() {
  registry_ = wl_display_get_registry(display_);
  if (!registry_) {
    throw WaylandError("Failed to get Wayland registry.");
  }
  wl_registry_add_listener(registry_, &REGISTRY_LISTENER, this);

  // Block until the server has sent all current globals so
  // compositor_/shm_/wm_base_ are populated before we proceed.
  wl_display_roundtrip(display_);

  if (!compositor_ || !shm_ || !wm_base_) {
    throw WaylandError(
        "Missing required Wayland globals (compositor / shm / xdg_wm_base). "
        "Is this actually running under a Wayland compositor?");
  }

  xdg_wm_base_add_listener(wm_base_, &WM_BASE_LISTENER, this);
}

void WaylandWindow::create_window(const std::string& application_name) {
  surface_ = wl_compositor_create_surface(compositor_);
  if (!surface_) {
    throw WaylandError("Failed to create wl_surface.");
  }

  xdg_surface_ = xdg_wm_base_get_xdg_surface(wm_base_, surface_);
  if (!xdg_surface_) {
    throw WaylandError("Failed to create xdg_surface.");
  }
  xdg_surface_add_listener(xdg_surface_, &XDG_SURFACE_LISTENER, this);

  xdg_toplevel_ = xdg_surface_get_toplevel(xdg_surface_);
  if (!xdg_toplevel_) {
    throw WaylandError("Failed to create xdg_toplevel.");
  }
  xdg_toplevel_add_listener(xdg_toplevel_, &XDG_TOPLEVEL_LISTENER, this);

  xdg_toplevel_set_title(xdg_toplevel_, application_name.c_str());
  xdg_toplevel_set_app_id(xdg_toplevel_, application_name.c_str());
}

void WaylandWindow::initial_commit() {
  // First commit with no buffer - this is what triggers the initial
  // xdg_surface `configure` event. Nothing is allowed to be visible
  // before this handshake completes.
  wl_surface_commit(surface_);
  wl_display_roundtrip(display_);

  if (!surface_configured) {
    throw WaylandError("Compositor never sent initial xdg_surface configure.");
  }

  buffer.emplace(shm_, width, height, 0xFF000000u /* opaque black */);

  wl_surface_attach(surface_, buffer->handle(), 0, 0);
  wl_surface_damage_buffer(surface_, 0, 0, width, height);
  wl_surface_commit(surface_);

  if (wl_display_flush(display_) < 0) {
    throw WaylandError("wl_display_flush failed after initial commit.");
  }
}

WaylandWindow::~WaylandWindow() {
  // Member destructors are run as the stack unwinds. The buffer must be freed
  // before the surface / shm are torn down. Then use our wl_*_destroy calls to
  // clean up our stuff. Use null checked calls since the constructor may have
  // thrown so setup did not finish.

  buffer.reset();

  if (xdg_toplevel_)
    xdg_toplevel_destroy(xdg_toplevel_);
  if (xdg_surface_)
    xdg_surface_destroy(xdg_surface_);
  if (surface_)
    wl_surface_destroy(surface_);
  if (wm_base_)
    xdg_wm_base_destroy(wm_base_);
  if (seat_)
    wl_seat_destroy(seat_);
  if (shm_)
    wl_shm_destroy(shm_);
  if (compositor_)
    wl_compositor_destroy(compositor_);
  if (registry_)
    wl_registry_destroy(registry_);
  if (display_)
    wl_display_disconnect(display_);
}

auto WaylandWindow::pump_messages() -> b8 {

  while (wl_display_prepare_read(display_) != 0) {
    // Events were already queued locally; drain those first, then
    // retry prepare_read (it fails if the queue isn't empty).
    if (wl_display_dispatch_pending(display_) < 0) {
      running = false;
      return false;
    }
  }

  // Flush any outgoing requests before we potentially block waiting for
  // incoming data.
  wl_display_flush(display_);

  pollfd pfd{};
  pfd.fd = wl_display_get_fd(display_);
  pfd.events = POLLIN;

  int ret = poll(&pfd, 1, 0); // 0ms timeout, non-blocking poll
  if (ret > 0 && (pfd.revents & POLLIN)) {
    if (wl_display_read_events(display_) < 0) {
      running = false;
      return false;
    }
  } else {
    // No data available (or poll error). Must cancel the read we
    // prepared above.
    wl_display_cancel_read(display_);
    if (ret < 0) {
      running = false;
      return false;
    }
  }

  if (wl_display_dispatch_pending(display_) < 0) {
    running = false;
    return false;
  }

  return running;
}

} // namespace static_eng::platform
// #endif