#
# Creator 2019-05-07 by Sven
#
# Set up Gtest libraries for the sample app.
#
# To build with Gtest, run the following command,
#     cmake <path-to-source> 
#       -DGTEST_ENABLE=ON 
#           -DGTEST_LIB_DIR=<path-to-googletest-lib-dir> 
#           -DGTEST_INCLUDE_DIR=<path-to-googletest-include-dir>
#

option(GTEST_ENABLE "Enable googletest for the sample test" OFF)

if(GTEST_ENABLE)
	if(NOT GTEST_LIB_DIR)
		message(FATAL_ERROR "Must pass library path of googletest to enable it.")
	endif()
	if(NOT GTEST_INCLUDE_DIR)
		message(FATAL_ERROR "Must pass include path of googletest to enable it.")
	endif()

	link_directories(${GTEST_LIB_DIR})
endif()

