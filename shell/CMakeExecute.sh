BUILD_TYPE="${1:-debug}"
TARGET="$2"
MEM_CHECK="$3"

if [ -z "$TARGET" ]; then
    echo "사용법: $0 [debug|release] <타겟명> [leaks]"
    echo "예시:  $0 debug OpenGL-ComputerGraphics"
    echo "       $0 debug LEGOSpiderman"
    exit 1
fi

if [ "$BUILD_TYPE" = "debug" ]; then
    BUILD_DIR="build_ninja"
elif [ "$BUILD_TYPE" = "release" ]; then
    BUILD_DIR="build_ninja-release"
else
    echo "사용법: $0 [debug|release] <타겟명> [leaks]"
    exit 1
fi

# 실행 파일 경로 (CMAKE_RUNTIME_OUTPUT_DIRECTORY = bin/)
EXEC_DIR="./$BUILD_DIR/bin"

if [ ! -f "$EXEC_DIR/$TARGET" ]; then
    echo "실행 파일을 찾을 수 없습니다: $EXEC_DIR/$TARGET"
    echo "빌드를 먼저 실행하세요."
    exit 1
fi

# 실행 파일 디렉토리로 이동 후 실행 (리소스 상대경로 해결)
pushd "$EXEC_DIR" > /dev/null
if [ "$MEM_CHECK" = "leaks" ]; then
    MallocStackLogging=1 leaks --atExit --list -- "./$TARGET"
else
    "./$TARGET"
fi
popd > /dev/null
