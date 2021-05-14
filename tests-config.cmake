include_guard()

include(FetchContent)

###########################################
# Fetch dependencies
###########################################
FetchContent_Declare(ez-catch
    GIT_REPOSITORY git@github.com:unixod/ez-catch.git
    GIT_TAG eeb58530558c6dde49bb6837a738b315dbbcc220
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
