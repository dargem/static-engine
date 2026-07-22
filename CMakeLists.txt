cmake_minimum_required(VERSION 3.5)

project(static_engine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(Vulkan REQUIRED)
find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE engine_sources engine/src/* engine/include/*)
# list(REMOVE_ITEM engine_sources "engine/src/main.cpp") # Remove main from the library

file(GLOB_RECURSE sources_test engine/test/*)
file(GLOB_RECURSE data engine/resources/*)

# Library for engine code
add_library(engine_lib STATIC ${engine_sources})
target_include_directories(engine_lib PUBLIC 
    engine/src 
    engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
)
target_link_libraries(engine_lib PRIVATE Vulkan::Vulkan)
target_compile_options(engine_lib PUBLIC -Wall -Wfloat-conversion)
target_compile_definitions(engine_lib PUBLIC CONFIGS_FILE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/engine/resources/configs.txt")

# Add definitions for platform it is compiled on
pkg_check_modules(WAYLAND wayland-client)

if (WAYLAND_FOUND)
    target_compile_definitions(engine_lib PRIVATE HAVE_WAYLAND=1)
    target_link_libraries(engine_lib PRIVATE ${WAYLAND_LIBRARIES})
    target_include_directories(engine_lib PRIVATE ${WAYLAND_INCLUDE_DIRS})
else()
    target_compile_definitions(engine_lib PRIVATE HAVE_WAYLAND=0)
endif()

# Main executable
add_executable(static_eng engine/src/main.cpp)
target_link_libraries(static_eng PRIVATE engine_lib)
file(COPY ${data} DESTINATION resources)

install(TARGETS static_eng DESTINATION engine)
install(DIRECTORY resources DESTINATION engine)

# Test Executable
add_executable(unit_tests ${sources_test})
target_link_libraries(unit_tests PRIVATE 
    GTest::gtest_main
    engine_lib
)

set(CPACK_PACKAGE_NAME "MyExample")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)