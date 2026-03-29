# ______ 챕터 이름 자동 추출 ______
get_filename_component(CHAPTER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# ______ 실행 파일 ______
add_executable(${CHAPTER_NAME} main.cpp)
set_project_standards(${CHAPTER_NAME})
target_include_directories(${CHAPTER_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${CHAPTER_NAME} PRIVATE project_deps project_config)

if(APPLE)
    target_compile_definitions(${CHAPTER_NAME} PRIVATE GL_SILENCE_DEPRECATION)
endif()

# ______ 빌드 후 DLL 복사 (Windows) ______
if(WIN32 AND COMMAND copy_freeglut_dll)
    copy_freeglut_dll(${CHAPTER_NAME})
endif()
