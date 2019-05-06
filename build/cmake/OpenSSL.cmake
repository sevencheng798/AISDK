#
# Set up Openssl libraries for the sample app.
#
# To build with PortAudio, run the following command,
#     cmake <path-to-source> 
#       -DOPENSSL=ON 
#           -DOPENSSL_LIB_PATH=<path-to-openssl-lib> 
#           -DOPENSSL_INCLUDE_DIR=<path-to-openssl-include-dir>
#

option(OPENSSL "Enable OPENSSL for the PortAudioMediaPlayer share library" OFF)

if(OPENSSL)
    if(NOT OPENSSL_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of Openssl to enable it.")
    endif()
    if(NOT OPENSSL_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of Openssl to enable it.")
    endif()
	
	link_directories(${OPENSSL_LIB_PATH})
endif()

