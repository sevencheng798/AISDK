#
# Setup the build type and compiler options.
#
# To set the build type, run the following command with a build type of DEBUG, RELEASE, or MINSIZEREL:
#     cmake <path-to-source> -DCMAKE_BUILD_TYPE=<build-type>
#

# If no build type is specified by specifying it on the command line, default to debug.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: DEBUG, RELEASE, or MINSIZEREL." FORCE)
    message("No build type specified, defaulting to Release.")
endif()

# Determine the platform and compiler dependent flags.
if (UNIX OR CMAKE_COMPILER_IS_GNUCXX)
    set(CXX_PLATFORM_DEPENDENT_FLAGS_DEBUG      "-DDEBUG -DAISDK_DEBUG_LOG_ENABLED -Wall -Werror -Wsign-compare -g")
    set(CXX_PLATFORM_DEPENDENT_FLAGS_RELEASE    "-DNDEBUG -Wall -Werror -O2")
    set(CXX_PLATFORM_DEPENDENT_FLAGS_MINSIZEREL "-DNDEBUG -Wall -Werror -Os")
    if (CMAKE_COMPILER_IS_GNUCXX AND (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")))
        set(CXX_PLATFORM_DEPENDENT_FLAGS_DEBUG "${CXX_PLATFORM_DEPENDENT_FLAGS_DEBUG} -Wno-error=class-memaccess")
        set(CXX_PLATFORM_DEPENDENT_FLAGS_RELEASE "${CXX_PLATFORM_DEPENDENT_FLAGS_RELEASE} -Wno-error=class-memaccess")
        set(CXX_PLATFORM_DEPENDENT_FLAGS_MINSIZEREL "${CXX_PLATFORM_DEPENDENT_FLAGS_MINSIZEREL} -Wno-error=class-memaccess")
    endif()
endif()

# Debug build, default.
set(CMAKE_CXX_FLAGS_DEBUG "${CXX_PLATFORM_DEPENDENT_FLAGS_DEBUG} -DRAPIDJSON_HAS_STDSTRING" CACHE INTERNAL "Flags used for DEBUG builds" FORCE)
set(CMAKE_C_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} CACHE INTERNAL "Flags used for DEBUG builds" FORCE)

# Release build.
set(CMAKE_CXX_FLAGS_RELEASE "${CXX_PLATFORM_DEPENDENT_FLAGS_RELEASE} -DRAPIDJSON_HAS_STDSTRING" CACHE INTERNAL "Flags used for RELEASE builds" FORCE)
set(CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} CACHE INTERNAL "Flags used for RELEASE builds" FORCE)

# Minimum sized release build.
#set(CMAKE_CXX_FLAGS_MINSIZEREL "${CXX_PLATFORM_DEPENDENT_FLAGS_MINSIZEREL} -DRAPIDJSON_HAS_STDSTRING" CACHE INTERNAL "Flags used for minimum sized RELEASE builds" FORCE)
#set(CMAKE_C_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_RELEASE} CACHE INTERNAL "Flags used for minimum sized RELEASE builds" FORCE)

