include_guard()

include(FetchContent)

###########################################
# Fetch dependencies
###########################################
FetchContent_Declare(ez-catch
    GIT_REPOSITORY https://github.com/unixod/ez-catch.git
    GIT_TAG v3.2.1
    GIT_SHALLOW On
)

FetchContent_MakeAvailable(ez-catch)

###########################################
# Add ez::tests_config
###########################################
add_library(ez_utils_tests_config INTERFACE)
add_library(ez::utils::tests_config ALIAS ez_utils_tests_config)

target_link_libraries(ez_utils_tests_config
    INTERFACE
    ez::catch)
