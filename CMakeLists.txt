cmake_minimum_required(VERSION 3.24)

project(ez-utils)

include(GNUInstallDirs)
include(CMakeDependentOption)

##############################################################################
# Setup project general properties
##############################################################################

# Check if this is a standalone project or included by other project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(EZ_UTILS_STANDALONE On)
    include(CTest)
endif()

# If ez-utils is used by some outer project (e.g. via CMake FetchContent or add_subdirectory)
# then don't register targets defined here to be installed until it is explicitelly requested
# by the outer project by setting EZ_UTILS_INSTALL option to On.
cmake_dependent_option(EZ_UTILS_INSTALL
    "Generate an install target for ez::utils" Off "NOT EZ_UTILS_STANDALONE" On)

# Require C++20 or higher.
# Notes:
# - CMAKE_CXX_STANDARD is for dependencies (for now it's only Catch2).
# - cxx_std_${EZ_UTILS_CPP_MINIMUM_REQUIRED} (in compile-options.cmake) is for ez-support and for
#   its consumers (i.e. dependent targets).
set(EZ_UTILS_CPP_MINIMUM_REQUIRED 20)
if(CMAKE_CXX_STANDARD VERSION_LESS ${EZ_UTILS_CPP_MINIMUM_REQUIRED})
    set(CMAKE_CXX_STANDARD ${EZ_UTILS_CPP_MINIMUM_REQUIRED})
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(dependencies.cmake)
include(compile-options.cmake)

# Enable custom build configurations
if (EZ_UTILS_STANDALONE)
    ez_common_enable_build_profile_debug_with_ubasan()
endif()

##############################################################################
# Setup ez::utils target
##############################################################################

add_library(ez_utils INTERFACE)
add_library(ez::utils ALIAS ez_utils)

target_sources(ez_utils
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ez/utils/recursive_generator.h
        include/ez/utils/generator.h
        include/ez/utils/enum-arithmetic.h
        include/ez/utils/match.h
        include/ez/utils/badge.h
        include/ez/utils/type-conversion.h
)

target_link_libraries(ez_utils
    INTERFACE
    ez::utils::public_compile_options
    ez::support
)

if(EZ_UTILS_INSTALL)
    # Note: an export 'ez-libs' is defined at ez-support.
    install(TARGETS ez_utils EXPORT ez-libs
        FILE_SET HEADERS
    )

    set_target_properties(ez_utils
        PROPERTIES
        EXPORT_NAME utils
    )
endif()

##############################################################################
# Setup unit tests
##############################################################################
if (EZ_UTILS_STANDALONE AND BUILD_TESTING)
    include(tests-config.cmake)
    add_subdirectory(tests)
endif()
