ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_TYPE="${1:-debug}"
TARGET_KIND="${2:-}" # tutorials, samples, 또는 빈 값(기본 apps만)
CLEAN="${3:-}"       # "clean" 입력 시 캐시 초기화

if [ "$BUILD_TYPE" = "debug" ]; then
    PRESET="ninja"
elif [ "$BUILD_TYPE" = "release" ]; then
    PRESET="ninja-release"
else
    echo "사용법: $0 [debug|release] [tutorials|samples] [clean]"
    exit 1
fi

# 타겟 종류에 따라 preset 선택
if [ -n "$TARGET_KIND" ]; then
    PRESET="ninja-${TARGET_KIND}"
fi

# 캐시 초기화 (--fresh: 캐시만 삭제, _deps/는 유지)
if [ "$CLEAN" = "clean" ]; then
    echo "CMake 캐시 초기화 (--fresh)"
    cmake --preset "$PRESET" --fresh
else
    cmake --preset "$PRESET"
fi

# clangd용 compile_commands.json 심볼릭 링크 갱신
BUILD_DIR="$ROOT_DIR/build_ninja"
if [ -f "$BUILD_DIR/compile_commands.json" ]; then
    ln -sf "$BUILD_DIR/compile_commands.json" "$ROOT_DIR/compile_commands.json"
fi
