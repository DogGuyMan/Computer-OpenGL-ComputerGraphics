ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_TYPE="${1:-debug}"
TARGET_KIND="${2:-}" # tutorials, samples, 또는 빈 값(기본 apps)
TARGET="${3:-OpenGL-ComputerGraphics}" # 기본값: 프로젝트 실행파일
MEM_CHECK="$4"

# 빌드 디렉토리 조합
if [ "$BUILD_TYPE" = "debug" ]; then
    BUILD_DIR="$ROOT_DIR/build_ninja"
elif [ "$BUILD_TYPE" = "release" ]; then
    BUILD_DIR="$ROOT_DIR/build_ninja"
else
    echo "사용법: $0 [debug|release] [tutorials|samples] [타겟명] [leaks]"
    exit 1
fi

EXEC_DIR="$BUILD_DIR/bin"

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
