#pragma once

#include "defines.hpp"
#include <core/logger.hpp>
#include <string_view>

namespace static_eng::platform {

struct PlatformState {
  void* internal_state;
} platform_state;

auto platform_startup(PlatformState* plat_state,
                      std::string_view application_name, i32 x, i32 y,
                      i32 width, i32 height) -> b8;

void platform_shutdown(PlatformState* plat_state);

auto platform_allocate(u64 size, b8 aligned) -> void*;
void platform_free(void* block, b8 aligned);
auto platform_zero_memory(void* block, u64 size) -> void*;
auto platform_copy_memory(void* dest, const void* source, u64 size) -> void*;
auto platform_set_memory(void* dest, i32 value, u64 size);

void platform_console_write(std::string_view message, u8 colour);
void platform_console_write_error(std::string_view message, u8 colour);

auto platform_get_absolute_time() -> f64;

void platform_sleep(u64 ms);

} // namespace static_eng::platform