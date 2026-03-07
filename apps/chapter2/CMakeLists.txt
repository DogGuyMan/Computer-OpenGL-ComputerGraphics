# ______ 챕터 이름 자동 추출 ______
get_filename_component(CHAPTER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# ______ 정적 라이브러리 (entry.cpp 등) ______
add_library(${CHAPTER_NAME}_lib STATIC entry.cpp)
set_project_standards(${CHAPTER_NAME}_lib)
target_include_directories(${CHAPTER_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${CHAPTER_NAME}_lib PUBLIC project_deps project_config)

if(APPLE)
    target_compile_definitions(${CHAPTER_NAME}_lib PUBLIC GL_SILENCE_DEPRECATION)
endif()

# ______ 챕터 전용 내부 모듈 (선택) ______
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt)
    add_subdirectory(src)
    if(TARGET ${CHAPTER_NAME}_src)
        target_link_libraries(${CHAPTER_NAME}_lib PRIVATE ${CHAPTER_NAME}_src)
    endif()
endif()

# ______ 실행 파일 ______
add_executable(${CHAPTER_NAME} main.cpp)
set_project_standards(${CHAPTER_NAME})
target_link_libraries(${CHAPTER_NAME} PRIVATE ${CHAPTER_NAME}_lib)

# ______ 빌드 후 DLL 복사 (Windows) ______
if(WIN32 AND COMMAND copy_freeglut_dll)
    copy_freeglut_dll(${CHAPTER_NAME})
endif()

# ______ 빌드 후 리소스 복사 ______
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_command(TARGET ${CHAPTER_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${CHAPTER_NAME}>/resources
        COMMENT "${CHAPTER_NAME}: 리소스를 빌드 디렉터리에 복사")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    add_custom_command(TARGET ${CHAPTER_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${CHAPTER_NAME}>/shaders
        COMMENT "${CHAPTER_NAME}: 셰이더를 빌드 디렉터리에 복사")
endif()
