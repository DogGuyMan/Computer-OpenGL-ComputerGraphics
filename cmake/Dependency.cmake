include(FetchContent)

set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)

# ______ OpenGL (시스템) ______
find_package(OpenGL REQUIRED)

# ______ GLUT (플랫폼별) ______
if(WIN32)
    # Windows (MSVC) — freeglut 동적 라이브러리
    set(FREEGLUT_ROOT ${EXTERN_DIR}/freeglut)

    add_library(freeglut_local SHARED IMPORTED)
    set_target_properties(freeglut_local PROPERTIES
        IMPORTED_LOCATION             ${FREEGLUT_ROOT}/bin/x64/freeglut.dll
        IMPORTED_IMPLIB               ${FREEGLUT_ROOT}/lib/x64/freeglut.lib
        INTERFACE_INCLUDE_DIRECTORIES ${FREEGLUT_ROOT}/include
    )
    set(GLUT_TARGET freeglut_local)

    # DLL 복사 헬퍼 함수
    function(copy_freeglut_dll target)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${FREEGLUT_ROOT}/bin/x64/freeglut.dll $<TARGET_FILE_DIR:${target}>
            COMMENT "${target}: freeglut.dll 복사")
    endfunction()
else()
    find_package(GLUT REQUIRED)
    set(GLUT_TARGET GLUT::GLUT)
endif()

# ______ GLM (FetchContent) ______
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)

# ______ spdlog (FetchContent) ______
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.1
    GIT_SHALLOW    TRUE
)

# ______ stb (FetchContent, 헤더 온리) ______
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)

# ______ FetchContent 대상 일괄 다운로드 ______
FetchContent_MakeAvailable(glm spdlog stb)

# ______ INTERFACE 타겟 수동 생성 ______
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# ______ 프로젝트 의존성 ______
add_library(project_deps INTERFACE)
target_link_libraries(project_deps INTERFACE
    OpenGL::GL
    OpenGL::GLU
    ${GLUT_TARGET}
    glm::glm
    spdlog::spdlog
    stb
)

# ______ 플랫폼별 라이브러리 ______
if(WIN32)
    target_link_libraries(project_deps INTERFACE
        opengl32
        gdi32
        winmm
    )
endif()
