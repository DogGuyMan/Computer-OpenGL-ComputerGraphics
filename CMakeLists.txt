# ______ 최소 CMake 버젼 요구사항 명시 ______
cmake_minimum_required(VERSION 3.14.0)

# ______ 프로젝트 이름 지정 ______
project(OpenGL-ComputerGraphics VERSION 0.1.0 LANGUAGES C CXX)

# ______ CMake 헬퍼 모듈 ______
include(cmake/CXXStandard.cmake)
include(cmake/Dependency.cmake)

# ______ 설정 헤더 (config.hpp) ______
add_subdirectory(config)

# ______ Source modules (공용 라이브러리, 존재 시) ______
if(EXISTS ${CMAKE_SOURCE_DIR}/src/CMakeLists.txt)
    add_subdirectory(src)
endif()

# ______ 챕터별 실행 파일 ______
# add_subdirectory(apps)
add_subdirectory(samples)
