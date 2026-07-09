# ______ 타겟에 프로젝트 표준 C++17 + 경고 옵션 적용 ______
function(set_project_standards target)
    target_compile_features(${target} PRIVATE cxx_std_17)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)

    set(_is_msvc "$<CXX_COMPILER_ID:MSVC>")
    set(_is_debug_msvc "$<AND:$<CONFIG:Debug>,${_is_msvc}>")
    set(_is_debug_gcc "$<AND:$<CONFIG:Debug>,$<NOT:${_is_msvc}>>")
    set(_no_error_pound "-Wno-error=#warnings")

    # ______ macOS OpenGL deprecation 경고 억제 ______
    target_compile_definitions(${target} PRIVATE
        $<$<PLATFORM_ID:Darwin>:GL_SILENCE_DEPRECATION>
    )

    target_compile_options(${target} PRIVATE
        # ______ 공통 경고 ______
        $<${_is_msvc}:           /W4 /permissive- /utf-8>
        $<$<NOT:${_is_msvc}>:   -Wall>
        # ______ Debug: warnings-as-errors + 세부 제어 ______
        $<${_is_debug_msvc}:     /WX /wd4100 /wd4101 /wd4189 /wd4244 /wd4459 /Od /Zi>
        $<${_is_debug_gcc}:
            -Werror -g -O0
            -Wconversion -Warray-bounds
            -Wunused-but-set-variable
            -Wno-unused-variable -Wno-error=unused-parameter
            -Wno-error=unused-but-set-variable -Wno-unused-function
            ${_no_error_pound}>
    )
endfunction()
