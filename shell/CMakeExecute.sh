ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_TYPE="${1:-debug}"
TARGET="$2"
MEM_CHECK="$3"

if [ -z "$TARGET" ]; then
    echo "사용법: $0 [debug|release] <타겟명> [leaks]"
    echo "예시:  $0 debug chapter1"
    exit 1
fi

if [ "$BUILD_TYPE" = "debug" ]; then
    BUILD_DIR="$ROOT_DIR/build_ninja"
elif [ "$BUILD_TYPE" = "release" ]; then
    BUILD_DIR="$ROOT_DIR/build_ninja-release"
else
    echo "사용법: $0 [debug|release] <타겟명> [leaks]"
    exit 1
fi

# apps/ 또는 samples/ 에서 타겟 탐색
if [ -d "$BUILD_DIR/apps/$TARGET" ]; then
    EXEC_DIR="$BUILD_DIR/apps/$TARGET"
elif [ -d "$BUILD_DIR/samples/$TARGET" ]; then
    EXEC_DIR="$BUILD_DIR/samples/$TARGET"
else
    echo "타겟을 찾을 수 없습니다: $TARGET"
    echo "검색 경로: $BUILD_DIR/apps/$TARGET, $BUILD_DIR/samples/$TARGET"
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
