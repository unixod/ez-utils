include_guard()

###############################################################################
# Define ez::utils::private_compile_options - a target representing a set of
# project common compile options/requirements intended for internal use only.
#
# This set encompases options which are good for development of the project
# itself but which aren't prerequisits for its use by consumers. I.e. this set
# may have -Werror -Wextra, etc. which are OK for this project but which might
# be to restrictive or inadmissible for consumers. Hence this options don't get
# into export set (ez-libs).
###############################################################################

add_library(ez_utils_private_compile_options INTERFACE)
add_library(ez::utils::private_compile_options ALIAS ez_utils_private_compile_options)

# Use ez::common::compile_options (from ez-common) as a basis for a set of private compile options
# for this project.
target_link_libraries(ez_utils_private_compile_options
    INTERFACE
    ez::common::compile_options
)

###############################################################################
# Define ez::utils::public_compile_options - a target representing a set of
# project common compile options/requiements which are for both internal use
# and for use by consumers. I.e. these options are exported (get to ez-libs
# export set) and thereby are imposed to consumers.
#
# This set forms a compliment to ez::utils::private_compile_options (see
# above).
###############################################################################

add_library(ez_utils_public_compile_options INTERFACE)
add_library(ez::utils::public_compile_options ALIAS ez_utils_public_compile_options)

# Make a requirement on minimal version of C++ Standard for this project be exportable.
target_compile_features(ez_utils_public_compile_options
    INTERFACE
    cxx_std_${EZ_UTILS_CPP_MINIMUM_REQUIRED}
)

# Export the project public compile options.
install(TARGETS ez_utils_public_compile_options EXPORT ez-libs)
set_target_properties(ez_utils_public_compile_options
    PROPERTIES
    EXPORT_NAME utils_public_compile_options
)
