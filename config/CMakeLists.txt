configure_file(
    "config.hpp.in"
    "${CMAKE_BINARY_DIR}/configured_files/include/config.hpp" ESCAPE_QUOTES
)

# INTERFACE 타겟 — 소비자에게 config.hpp 경로만 전파
add_library(project_config INTERFACE)
target_include_directories(project_config INTERFACE
    ${CMAKE_BINARY_DIR}/configured_files/include
)
