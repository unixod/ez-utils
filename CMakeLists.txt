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

##############################################################################
# Fetch dependencies
##############################################################################
include(FetchContent)

FetchContent_Declare(ez-support
    GIT_REPOSITORY https://github.com/unixod/ez-support.git
    GIT_TAG e1157da0d3ce2a253aabd63d72cd50e9404067a6
    GIT_SHALLOW On)

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
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(ez_utils
    INTERFACE
    ez::utils::compile_options
    ez::support)

install(DIRECTORY include/ez TYPE INCLUDE)
install(TARGETS ez_utils EXPORT ez-libs)
set_target_properties(ez_utils
    PROPERTIES
    EXPORT_NAME utils)


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


