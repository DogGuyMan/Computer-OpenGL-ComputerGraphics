# ______ 타겟에 프로젝트 표준 C++17 + 경고 옵션 적용 ______
function(set_project_standards target)
    target_compile_features(${target} PRIVATE cxx_std_17)
    set_target_properties(${target} PROPERTIES
        CXX_EXTENSIONS OFF
    )
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wformat=2
        -Wcast-align
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
    )
endfunction()
