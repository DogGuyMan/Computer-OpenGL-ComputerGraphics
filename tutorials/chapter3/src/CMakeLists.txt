# ______ 챕터 전용 내부 모듈 ______
file(GLOB_RECURSE CHAPTER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

if(CHAPTER_SRC_FILES)
    add_library(${CHAPTER_NAME}_src STATIC ${CHAPTER_SRC_FILES})
    set_project_standards(${CHAPTER_NAME}_src)
    target_include_directories(${CHAPTER_NAME}_src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${CHAPTER_NAME}_src PUBLIC project_deps)
endif()
