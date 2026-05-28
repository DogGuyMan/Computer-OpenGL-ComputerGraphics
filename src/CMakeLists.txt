# ______ 주 모듈 라이브러리 ______
file(GLOB_RECURSE project_srcs CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

if(project_srcs)
    add_library(project_srcs STATIC ${project_srcs})
    set_project_standards(project_srcs)
    target_include_directories(project_srcs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(project_srcs PUBLIC project_deps project_config)
endif()