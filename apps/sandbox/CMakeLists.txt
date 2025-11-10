add_executable(OnionSandbox main.cpp)
target_link_libraries(OnionSandbox PRIVATE onion::engine)

# Copy assets next to the binary for runtime
add_custom_command(TARGET OnionSandbox POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:OnionSandbox>/assets)