# ______ 최소 CMake 버젼 요구사항 명시 ______
cmake_minimum_required(VERSION 3.14.0)

# ______ 프로젝트 이름 지정 ______
project(OpenGL-ComputerGraphics VERSION 0.1.0 LANGUAGES C CXX)

# ______ 빌드 타겟 옵션 (Preset에서 제어, 배타적) ______
option(BUILD_TUTORIALS "tutorials/ 빌드 (apps 대신)" OFF)
option(BUILD_SAMPLES   "samples/ 빌드 (apps 대신)"   OFF)

# ______ 실행파일 출력 디렉토리 통합 ______
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ______ CMake 헬퍼 모듈 ______
include(cmake/CXXStandard.cmake)
include(cmake/Dependency.cmake)

# ______ 설정 헤더 (config.hpp) ______
add_subdirectory(config)

# ______ 주 모듈 라이브러리 (src/) ______
add_subdirectory(src)

# ______ 빌드 타겟 분기 (배타적) ______
if(BUILD_TUTORIALS AND EXISTS ${CMAKE_SOURCE_DIR}/tutorials/CMakeLists.txt)
    add_subdirectory(tutorials)
elseif(BUILD_SAMPLES AND EXISTS ${CMAKE_SOURCE_DIR}/samples/CMakeLists.txt)
    add_subdirectory(samples)
else()
    add_subdirectory(apps)
endif()
