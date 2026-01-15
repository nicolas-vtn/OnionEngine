cmake_minimum_required(VERSION 3.24)

project(OnionEngine
  VERSION 0.1.0
  DESCRIPTION "A layered C++20 3D engine (OpenGL + GLFW)"
  LANGUAGES CXX C)

# Options
option(ONION_BUILD_SANDBOX "Build sandbox app" ON)
option(ONION_BUILD_TESTS   "Build tests" OFF)
#option(ONION_USE_SYSTEM_DEPS "Prefer system packages over FetchContent" OFF)
option(ONION_ENABLE_WARNINGS "Enable strict warnings" ON)
#option(ONION_ENABLE_LTO "Enable link-time optimization if supported" OFF)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Dependencies (GLFW, GLAD, GLM, stb, Assimp optional)
include(FetchContent)


include(FetchGLFW)
include(FetchGLM)
include(FetchSTB)
include(FetchAssimp)

add_subdirectory(external/glad)

# Add engine library
add_subdirectory(src/onion)

# Apps / Examples
if(ONION_BUILD_SANDBOX)
  add_subdirectory(apps/sandbox)
endif()


# Tests
if(ONION_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()