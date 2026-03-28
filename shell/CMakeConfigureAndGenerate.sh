BUILD_TYPE="${1:-debug}"
TARGET_KIND="${2:-}" # tutorials, samples, 또는 빈 값(기본 apps만)

if [ "$BUILD_TYPE" = "debug" ]; then
    PRESET="ninja"
elif [ "$BUILD_TYPE" = "release" ]; then
    PRESET="ninja-release"
else
    echo "사용법: $0 [debug|release] [tutorials|samples]"
    exit 1
fi

# 타겟 종류에 따라 preset 선택
if [ -n "$TARGET_KIND" ]; then
    PRESET="ninja-${TARGET_KIND}"
fi

cmake --preset "$PRESET"
