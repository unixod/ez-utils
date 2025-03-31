include_guard()

include(FetchContent)

FetchContent_Declare(ez-common
    GIT_REPOSITORY https://github.com/unixod/ez-common.git
    GIT_TAG v1.2.2
    GIT_SHALLOW On
)

FetchContent_Declare(ez-support
    GIT_REPOSITORY https://github.com/unixod/ez-support.git
    GIT_TAG v1.1.1
    GIT_SHALLOW On
)

# Enable installaling of ez::support if installating of ez::utils is enabled.
set(EZ_SUPPORT_INSTALL ${EZ_UTILS_INSTALL})
FetchContent_MakeAvailable(ez-common ez-support)

if(EZ_UTILS_STANDALONE AND BUILD_TESTING)
    FetchContent_Declare(ez-catch
        GIT_REPOSITORY https://github.com/unixod/ez-catch.git
        GIT_TAG v3.8.0
        GIT_SHALLOW On
    )

    FetchContent_MakeAvailable(ez-catch)
endif()
