include_guard()

include(FetchContent)

###########################################
# Fetch dependencies
###########################################
FetchContent_Declare(ez-catch
    GIT_REPOSITORY git@github.com:unixod/ez-catch-add-test.git
    GIT_TAG 76b1d0268c02514da30f5d8e6f93431a6cac5cbc
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
