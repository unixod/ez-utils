include_guard()

###############################################################################
# Define ez::tests_config
###############################################################################

add_library(ez_utils_tests_config INTERFACE)
add_library(ez::utils::tests_config ALIAS ez_utils_tests_config)

target_link_libraries(ez_utils_tests_config
    INTERFACE
    ez::catch
    ez::utils::private_compile_options
)
