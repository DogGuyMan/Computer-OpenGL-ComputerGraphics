# ______ 메인 애플리케이션 ______
add_executable(${PROJECT_NAME} main.cpp)
set_project_standards(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE project_deps project_config)

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)
endif()

# ______ 빌드 후 리소스 복사 ______
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
    COMMENT "${PROJECT_NAME} 빌드 후 리소스를 build 디렉터리에 복사"
)

# ______ 과제 디렉토리 (각 과제에 CMakeLists.txt 추가 시 활성화) ______
# add_subdirectory(assignment1)
# add_subdirectory(assignment2)
# add_subdirectory(assignment3)
