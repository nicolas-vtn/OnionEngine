# FetchImGui.cmake
# ---------------------------------------------------------------------------

if (TARGET imgui::imgui)
    return()
endif()

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.1
)

FetchContent_MakeAvailable(imgui)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

set(IMGUI_BACKEND_SOURCES
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)

add_library(imgui::imgui ALIAS imgui)

target_include_directories(imgui
    PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC OpenGL::GL glfw)
