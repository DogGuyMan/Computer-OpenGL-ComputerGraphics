include(FetchContent)

set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)

# ______ OpenGL (시스템) ______
find_package(OpenGL REQUIRED)

# ______ GLUT (플랫폼별) ______
if(WIN32)
    # Windows — extern/freeglut 정적 라이브러리 사용
    set(FREEGLUT_ROOT ${EXTERN_DIR}/freeglut)
    add_library(freeglut_local STATIC IMPORTED)
    set_target_properties(freeglut_local PROPERTIES
        IMPORTED_LOCATION       ${FREEGLUT_ROOT}/lib/libfreeglut_static.a
        INTERFACE_INCLUDE_DIRECTORIES ${FREEGLUT_ROOT}/include
        INTERFACE_COMPILE_DEFINITIONS FREEGLUT_STATIC
    )
    target_link_libraries(freeglut_local INTERFACE winmm gdi32)
    set(GLUT_TARGET freeglut_local)
else()
    find_package(GLUT REQUIRED)
    set(GLUT_TARGET GLUT::GLUT)
endif()

# ______ GLFW (FetchContent) ______
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)

# ______ GLM (FetchContent) ______
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)

# ______ gl3w (FetchContent) ______
FetchContent_Declare(gl3w
    GIT_REPOSITORY https://github.com/skaslev/gl3w.git
    GIT_TAG        master
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
FetchContent_MakeAvailable(glfw glm gl3w spdlog stb)

# ______ INTERFACE 타겟 수동 생성 ______
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# ______ 프로젝트 의존성 ______
add_library(project_deps INTERFACE)
target_link_libraries(project_deps INTERFACE
    OpenGL::GL
    OpenGL::GLU
    ${GLUT_TARGET}
    glfw
    glm::glm
    gl3w
    spdlog::spdlog
    stb
)

# ______ 플랫폼별 프레임워크/라이브러리 ______
if(APPLE)
    target_link_libraries(project_deps INTERFACE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
    )
elseif(WIN32)
    target_link_libraries(project_deps INTERFACE
        opengl32
        gdi32
        winmm
    )
endif()
