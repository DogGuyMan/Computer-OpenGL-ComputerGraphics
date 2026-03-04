# ______ 최소 CMake 버젼 요구사항 명시 ______
cmake_minimum_required(VERSION 3.14.0)

# ______ 프로젝트 이름 지정 ______
project(OpenGL-ComputerGraphics VERSION 0.1.0 LANGUAGES C CXX)

# ______ CMake 헬퍼 모듈 ______
include(cmake/CXXStandard.cmake)
include(cmake/Dependency.cmake)

# ______ 설정 헤더 (config.hpp) ______
add_subdirectory(config)

# ______ Source modules (내부 라이브러리) ______
add_subdirectory(src)

# ______ 애플리케이션 (실행 파일 + 과제) ______
add_subdirectory(app)
