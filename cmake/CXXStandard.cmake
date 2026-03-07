# ______ 타겟에 프로젝트 표준 C++17 + 경고 옵션 적용 ______
function(set_project_standards target)
    target_compile_features(${target} PRIVATE cxx_std_17)
    set_target_properties(${target} PROPERTIES
        CXX_EXTENSIONS OFF
    )

    # ______ 공통 경고 ______
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>: /W4 /permissive- /utf-8>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Wall -Wextra -Wpedantic>
    )

    # ______ Debug 전용: -Werror (unused-variable 제외) ______
    target_compile_options(${target} PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>: /WX /wd4101 /Od /Zi>
        $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:
            -Werror -Wno-error=unused-variable -Wno-error=unused-parameter -g -O0>
    )
endfunction()
