##################################################
# Define ez::utils::compile_options - a target
# representing a set of common compile options 
# for a project.
##################################################

include_guard()

add_library(ez_utils_compile_options INTERFACE)
add_library(ez::utils::compile_options ALIAS ez_utils_compile_options)

if(MSVC)
    target_compile_options(ez_utils_compile_options
        INTERFACE
        /EHsc
        /Wall
        /WX
        /Zc:__cplusplus
    )
elseif(EZ_UTILS_STANDALONE)
    target_compile_options(ez_utils_compile_options
        INTERFACE
        -Wall
        -Wcast-align
        -Wconversion
        -Werror
        -Wextra
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpedantic
        -Wshadow
        -Wsign-conversion
    )
endif()

target_compile_features(ez_utils_compile_options INTERFACE cxx_std_20)
