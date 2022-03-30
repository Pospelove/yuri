set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_OVERLAY_PORTS "${CMAKE_CURRENT_LIST_DIR}/overlay_ports")

cmake_minimum_required(VERSION 3.23)
project(yuri)

set(CMAKE_CXX_STANDARD 17)

include(cmake/CMakeRC.cmake)

add_library(vcpkg_deps INTERFACE)

find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(vcpkg_deps INTERFACE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(vcpkg_deps INTERFACE imgui::imgui)
target_link_libraries(vcpkg_deps INTERFACE SDL2::SDL2_image)
target_link_libraries(vcpkg_deps INTERFACE SDL2::SDL2_ttf)
target_link_libraries(vcpkg_deps INTERFACE glad::glad)

find_path(LUNASVG_INCLUDE_DIR NAMES lunasvg.h)
find_library(LUNASVG_LIBRARY_DEBUG NAMES lunasvg)
string(REPLACE "/debug/lib/" "/lib/" LUNASVG_LIBRARY_RELEASE "${LUNASVG_LIBRARY_DEBUG}")
target_link_libraries(vcpkg_deps INTERFACE "$<IF:$<CONFIG:Debug>,${LUNASVG_LIBRARY_DEBUG},${LUNASVG_LIBRARY_RELEASE}>")
target_include_directories(vcpkg_deps INTERFACE ${LUNASVG_INCLUDE_DIR})

find_path(STB_INCLUDE_DIR stb.h)
find_library(LUNASVG_LIBRARY_DEBUG NAMES stb)
target_include_directories(vcpkg_deps INTERFACE ${STB_INCLUDE_DIR})

file(GLOB_RECURSE asset_sources assets/*)
cmrc_add_resource_library(assets ${asset_sources})

file(GLOB_RECURSE sources src/*)
list(APPEND sources .clang-format)
add_executable(yuri ${sources})
target_link_libraries(yuri PUBLIC vcpkg_deps)
target_link_libraries(yuri PUBLIC assets)
