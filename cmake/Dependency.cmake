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
FetchContent_Declare(imgui     GIT_REPOSITORY https://github.com/ocornut/imgui.git     GIT_TAG docking GIT_SHALLOW TRUE)
FetchContent_Declare(imguizmo  GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git GIT_TAG master GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glm spdlog stb imgui)

# imguizmo: 업스트림이 자체 CMakeLists.txt 를 추가했지만, 본 프로젝트는 imgui 와 동일한 수동 빌드 방식을 유지한다.
# add_subdirectory 호출(=중복 imguizmo 타겟 생성)을 피하기 위해 Populate 만 수행한다.
FetchContent_GetProperties(imguizmo)
if(NOT imguizmo_POPULATED)
    FetchContent_Populate(imguizmo)
endif()

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE ${stb_SOURCE_DIR})

# imgui (코어 + demo + GLUT/OpenGL2 백엔드)
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glut.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
)
target_include_directories(imgui SYSTEM PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC
    OpenGL::GL OpenGL::GLU
    $<IF:$<BOOL:${WIN32}>,freeglut_local,GLUT::GLUT>
)
if(APPLE)
    target_compile_options(imgui PRIVATE -Wno-macro-redefined)
endif()

# imguizmo (업스트림 CMakeLists 무시, imgui 와 동일 방식으로 수동 빌드)
add_library(imguizmo STATIC
    ${imguizmo_SOURCE_DIR}/src/ImGuizmo.cpp
    ${imguizmo_SOURCE_DIR}/src/ImCurveEdit.cpp
    ${imguizmo_SOURCE_DIR}/src/ImGradient.cpp
    ${imguizmo_SOURCE_DIR}/src/ImSequencer.cpp
    ${imguizmo_SOURCE_DIR}/src/GraphEditor.cpp
    ${imguizmo_SOURCE_DIR}/src/ImVectorEditor.cpp
)
target_include_directories(imguizmo SYSTEM PUBLIC ${imguizmo_SOURCE_DIR}/src)
target_link_libraries(imguizmo PUBLIC imgui)

# ====== 프로젝트 의존성 통합 타겟 ======
add_library(project_deps INTERFACE)
target_link_libraries(project_deps INTERFACE
    OpenGL::GL OpenGL::GLU
    $<IF:$<BOOL:${WIN32}>,freeglut_local,GLUT::GLUT>
    glm::glm spdlog::spdlog stb imgui imguizmo
    $<$<BOOL:${WIN32}>:gdi32 winmm>
)
