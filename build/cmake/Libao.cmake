#
# Set up Libao specific configurations for the sample app.
#
# To build with Libao, run the following command,
#     cmake <path-to-source>
#       -DAO_LIB_PATH=<path-to-ao-lib>
#       -DAO_INCLUDE_DIR=<path-to-ao-include-dir>
#
option(AO_LIB_PATH, "Path to LIBAO shared libraries")
option(AO_INCLUDE_DIR, "Path to LIBAO include directory")

if (AO_LIB_PATH AND AO_INCLUDE_DIR)
    message("Including libao libraries")
    link_directories(${AO_LIB_PATH})
endif()
