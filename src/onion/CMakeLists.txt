add_library(onion_engine)
add_library(onion::engine ALIAS onion_engine)


target_sources(onion_engine
  PRIVATE
    core/engine.cpp
    renderer/renderer.cpp
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)


target_include_directories(onion_engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>)




# Link deps
# glfw/glad/glm/stb/assimp targets may differ depending on your fetch modules
target_link_libraries(onion_engine
  PUBLIC
     glfw
#    glad
#    glm
  PRIVATE
    $<$<BOOL:${ASSIMP_FOUND}>:assimp::assimp>)


# Platform libs
if(WIN32)
  target_link_libraries(onion_engine PRIVATE opengl32)
elseif(UNIX)
  find_package(OpenGL REQUIRED)
  target_link_libraries(onion_engine PRIVATE OpenGL::GL dl pthread)
endif()


# Compile defs
target_compile_definitions(onion_engine
  PUBLIC $<$<CONFIG:Debug>:ONION_DEBUG=1>)


# Warnings
if(ONION_ENABLE_WARNINGS)
  include(warnings)
  onion_enable_warnings(onion_engine)
endif()