BUILD_TYPE="${1:-debug}"
TARGET_KIND="${2:-}" # tutorials, samples, 또는 빈 값(기본 apps만)
TARGET="$3"          # 특정 타겟명 (선택)

if [ "$BUILD_TYPE" = "debug" ]; then
    PRESET="ninja"
elif [ "$BUILD_TYPE" = "release" ]; then
    PRESET="ninja-release"
else
    echo "사용법: $0 [debug|release] [tutorials|samples] [타겟명]"
    exit 1
fi

# 타겟 종류에 따라 preset 선택
if [ -n "$TARGET_KIND" ]; then
    PRESET="ninja-${TARGET_KIND}"
fi

if [ -n "$TARGET" ]; then
    cmake --build --preset "$PRESET" --target "$TARGET"
else
    cmake --build --preset "$PRESET"
fi
