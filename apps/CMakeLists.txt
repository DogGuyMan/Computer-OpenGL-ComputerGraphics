# ______ 실행 파일 (entry point) ______
add_executable(${CMAKE_PROJECT_NAME} main.cpp)
set_project_standards(${CMAKE_PROJECT_NAME})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Module project_config)

# ______ 빌드 후 DLL 복사 (Windows) ______
if(WIN32 AND COMMAND copy_freeglut_dll)
    copy_freeglut_dll(${CMAKE_PROJECT_NAME})
endif()

# ______ 빌드 후 리소스 복사 ______
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/resources
        COMMENT "${CMAKE_PROJECT_NAME}: 리소스를 빌드 디렉터리에 복사")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/shaders
        COMMENT "${CMAKE_PROJECT_NAME}: 셰이더를 빌드 디렉터리에 복사")
endif()
