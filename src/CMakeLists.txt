# ______ 주 모듈 라이브러리 ______
file(GLOB_RECURSE MODULE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 소스 파일이 없으면 빌드 스킵
if(NOT MODULE_SOURCES)
    # 헤더 온리 또는 아직 소스 없음 -> INTERFACE 타겟으로 대체
    add_library(Module INTERFACE)
    target_include_directories(Module INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(Module INTERFACE project_deps)
    if(APPLE)
        target_compile_definitions(Module INTERFACE GL_SILENCE_DEPRECATION)
    endif()
else()
    add_library(Module STATIC ${MODULE_SOURCES})
    set_project_standards(Module)
    target_include_directories(Module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(Module PUBLIC project_deps)
    if(APPLE)
        target_compile_definitions(Module PUBLIC GL_SILENCE_DEPRECATION)
    endif()
endif()
