BASE_DIR="$(dirname "$0")"
BUILD_TYPE="${1:-debug}"
TARGET_KIND="${2:-}" # tutorials, samples, 또는 빈 값
TARGET="$3"
MEM_CHECK="$4"

if [ -z "$TARGET" ]; then
    echo "사용법: $0 [debug|release] [tutorials|samples] <타겟명> [leaks]"
    echo "예시:  $0 debug \"\" OpenGL-ComputerGraphics"
    echo "       $0 debug samples LEGOSpiderman"
    exit 1
fi

sh "$BASE_DIR/CMakePrepare.sh"
sh "$BASE_DIR/CMakeConfigureAndGenerate.sh" "$BUILD_TYPE" "$TARGET_KIND"
sh "$BASE_DIR/CMakeBuild.sh"               "$BUILD_TYPE" "$TARGET_KIND" "$TARGET"
sh "$BASE_DIR/CMakeExecute.sh"             "$BUILD_TYPE" "$TARGET_KIND" "$TARGET" "$MEM_CHECK"
