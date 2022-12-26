include_guard()

if (EZ_UTILS_STANDALONE)
    # Make available DEBUG_WITH_UBASAN CMake build profile
    ez_common_enable_build_profile_debug_with_ubasan()
endif()
