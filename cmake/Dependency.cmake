include(FetchContent)

set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)

# ====== 시스템 라이브러리 ======
find_package(OpenGL REQUIRED)

if(WIN32)
    # freeglut (동적 라이브러리, extern/)
    set(FREEGLUT_ROOT ${EXTERN_DIR}/freeglut)
    add_library(freeglut_local SHARED IMPORTED)
    set_target_properties(freeglut_local PROPERTIES
        IMPORTED_LOCATION             ${FREEGLUT_ROOT}/bin/x64/freeglut.dll
        IMPORTED_IMPLIB               ${FREEGLUT_ROOT}/lib/x64/freeglut.lib
        INTERFACE_INCLUDE_DIRECTORIES ${FREEGLUT_ROOT}/include
    )

    # DLL 복사 헬퍼
    function(copy_freeglut_dll target)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${FREEGLUT_ROOT}/bin/x64/freeglut.dll $<TARGET_FILE_DIR:${target}>
            COMMENT "${target}: freeglut.dll 복사")
    endfunction()
else()
    find_package(GLUT REQUIRED)
endif()

# ====== FetchContent ======
set(GLM_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glm    GIT_REPOSITORY https://github.com/g-truc/glm.git    GIT_TAG 1.0.1   GIT_SHALLOW TRUE)
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.15.1 GIT_SHALLOW TRUE)
FetchContent_Declare(stb    GIT_REPOSITORY https://github.com/nothings/stb.git  GIT_TAG master  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glm spdlog stb)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# ====== 프로젝트 의존성 통합 타겟 ======
add_library(project_deps INTERFACE)
target_link_libraries(project_deps INTERFACE
    OpenGL::GL OpenGL::GLU
    $<IF:$<BOOL:${WIN32}>,freeglut_local,GLUT::GLUT>
    glm::glm spdlog::spdlog stb
    $<$<BOOL:${WIN32}>:gdi32 winmm>
)
