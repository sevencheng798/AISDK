#
# Setup the Keyword Detector type and compiler options.
#
# To build with a Keyword Detector, run the following command with a keyword detector type of IFLYTEK_AIUI_ASR or
# SOUNDAI_ASR:
#     cmake <path-to-source> 
#       -DIFLYTEK_AIUI_ASR=ON 
#           -DIFLYTEK_AIUI_ASR_LIB_PATH=<path-to-iflytek-lib>
#           -DIFLYTEK_AIUI_ASR_INCLUDE_DIR=<path-to-iflytek-include-dir>
#       -DSOUNDAI_ASR=ON 
#           -DSOUNDAI_ASR_LIB_PATH=<path-to-soundai-lib>
#           -DSOUNDAI_ASR_INCLUDE_DIR=<path-to-soundai-include-dir>
#

option(IFLYTEK_AIUI_ASR "Enable IflyTek aiui asr engine." OFF)
option(SOUNDAI_ASR "Enable SoundAi asr engine." OFF)

if(NOT IFLYTEK_AIUI_ASR AND NOT SOUNDAI_ASR)
    message("No ASR Enginer type specified, skipping build of ASR Enginer.")
    return()
endif()

if(IFLYTEK_AIUI_ASR)
    message("Creating ${PROJECT_NAME} with ASR Engine type: IflyTek AIUI")
    if(NOT IFLYTEK_AIUI_ASR_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of IflyTek(libaiui.so) AIUI Engine! You can set 'IFLYTEK_AIUI_ASR_LIB_PATH' to specify the path.")
    endif()
    if(NOT IFLYTEK_AIUI_ASR_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of IflyTek AIUI Engine! You can set 'IFLYTEK_AIUI_ASR_INCLUDE_DIR' to specify the path.")
    endif()
    add_definitions(-DENABLE_IFLYTEK_AIUI_ASR)
	link_directories("${IFLYTEK_AIUI_ASR_LIB_PATH}")
    set(ENABLE_IFLYTEK_AIUI_ASR ON)
endif()

if(SOUNDAI_ASR)
    message("Creating ${PROJECT_NAME} with ASR Engine type: SoundAi")
    if(NOT SOUNDAI_ASR_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of SoundAi Engine! You can set 'SOUNDAI_ASR_LIB_PATH' to specify the path.")
    endif()
    if(NOT SOUNDAI_ASR_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of SoundAi Engine! You can set 'SOUNDAI_ASR_INCLUDE_DIR' to specify the path.")
    endif()
    add_definitions(-DENABLE_SOUNDAI_ASR)
	add_definitions(-DENABLE_HTTP2)
	link_directories("${SOUNDAI_ASR_LIB_PATH}")
    set(ENABLE_SOUNDAI_ASR ON)
endif()
