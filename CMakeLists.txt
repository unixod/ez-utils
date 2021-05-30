cmake_minimum_required(VERSION 3.16)

project(ez-utils
    VERSION 1.0.0
    LANGUAGES CXX
)

include(GNUInstallDirs)

##############################################################################
# Setup project general properties
##############################################################################

# Check if this is a standalone project or included by other project
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(EZ_UTILS_STANDALONE On)
endif()

# Although this library is supposed to be dependency free its unit tests
# are based on Catch2 unit test framework. Specifying CMAKE_CXX_STANDARD
# in the following block is necessary for correct building of and linking
# with Catch2 and potentialy any other fetched dependency because they
# need to be informed somehow about the C++ standard being used.
if(EZ_UTILS_STANDALONE AND NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

option(EZ_UTILS_INSTALL "Enable installation of ez-utils target" EZ_UTILS_STANDALONE)

##############################################################################
# Fetch dependencies
##############################################################################
include(FetchContent)

FetchContent_Declare(ez-support
    GIT_REPOSITORY git@github.com:unixod/ez-support.git
    GIT_TAG 3561b5d3ae027d90f9407c83760993979af4fdd1
    GIT_SHALLOW On
)
FetchContent_MakeAvailable(ez-support)

##############################################################################
# Setup ez::utils target
##############################################################################
include(compile-options.cmake)

add_library(ez_utils INTERFACE)
add_library(ez::utils ALIAS ez_utils)

target_include_directories(ez_utils
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>)

target_link_libraries(ez_utils
    INTERFACE
    ez::utils::compile_options
    ez::support)

if(EZ_SUPPORT_INSTALL)
    install(TARGETS ez_support)
    install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

##############################################################################
# Setup unit tests
##############################################################################
if (EZ_UTILS_STANDALONE)
    include(CTest)

    if (BUILD_TESTING)
        include(tests-config.cmake)
        add_subdirectory(tests)
    endif()
endif()


