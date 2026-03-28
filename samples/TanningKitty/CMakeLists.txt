# ______ 샘플 이름 자동 추출 ______
get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# ______ 실행 파일 ______
add_executable(${SAMPLE_NAME} main.cpp)
set_project_standards(${SAMPLE_NAME})
target_link_libraries(${SAMPLE_NAME} PRIVATE project_deps project_config)

if(APPLE)
    target_compile_definitions(${SAMPLE_NAME} PRIVATE GL_SILENCE_DEPRECATION)
endif()

# ______ 빌드 후 DLL 복사 (Windows) ______
if(WIN32 AND COMMAND copy_freeglut_dll)
    copy_freeglut_dll(${SAMPLE_NAME})
endif()

# ______ 빌드 후 리소스 복사 ______
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Texture)
    add_custom_command(TARGET ${SAMPLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Texture $<TARGET_FILE_DIR:${SAMPLE_NAME}>/Texture
        COMMENT "${SAMPLE_NAME}: 리소스를 빌드 디렉터리에 복사")
endif()
