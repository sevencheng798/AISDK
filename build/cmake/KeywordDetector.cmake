#
# Setup the Keyword Detector type and compiler options.
#
# To build with a Keyword Detector, run the following command with a keyword detector type of IFLYTEK_KEY_WORD_DETECTOR or
# SOUNDAI_KEY_WORD_DETECTOR:
#     cmake <path-to-source> 
#       -DIFLYTEK_KEY_WORD_DETECTOR=ON 
#           -DIFLYTEK_KEY_WORD_DETECTOR_LIB_PATH=<path-to-iflytek-lib>
#           -DIFLYTEK_KEY_WORD_DETECTOR_INCLUDE_DIR=<path-to-iflytek-include-dir>
#       -DSOUNDAI_KEY_WORD_DETECTOR=ON 
#           -DSOUNDAI_KEY_WORD_DETECTOR_LIB_PATH=<path-to-soundai-lib>
#           -DSOUNDAI_KEY_WORD_DETECTOR_INCLUDE_DIR=<path-to-soundai-include-dir>
#

option(IFLYTEK_KEY_WORD_DETECTOR "Enable IflyTek keyword detector." OFF)
option(SOUNDAI_KEY_WORD_DETECTOR "Enable SoundAi keyword detector." OFF)

if(NOT IFLYTEK_KEY_WORD_DETECTOR AND NOT SOUNDAI_KEY_WORD_DETECTOR)
    message("No keyword detector type specified, skipping build of keyword detector.")
    return()
endif()

if(IFLYTEK_KEY_WORD_DETECTOR)
    message("Creating ${PROJECT_NAME} with keyword detector type: IflyTek")
    if(NOT IFLYTEK_KEY_WORD_DETECTOR_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of IflyTek(libmsc.so) KeywordDetector! You can set 'IFLYTEK_KEY_WORD_DETECTOR_LIB_PATH' to specify the path.")
    endif()
    if(NOT IFLYTEK_KEY_WORD_DETECTOR_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of IflyTek KeywordDetector! You can set 'IFLYTEK_KEY_WORD_DETECTOR_INCLUDE_DIR' to specify the path.")
    endif()
    add_definitions(-DKWD)
    add_definitions(-DKWD_IFLYTEK)
	link_directories("${IFLYTEK_KEY_WORD_DETECTOR_LIB_PATH}")
    set(KWD ON)
endif()

if(SOUNDAI_KEY_WORD_DETECTOR)
    message("Creating ${PROJECT_NAME} with keyword detector type: SoundAi")
    if(NOT SOUNDAI_KEY_WORD_DETECTOR_LIB_PATH)
        message(FATAL_ERROR "Must pass library path of SoundAi KeywordDetector! You can set 'SOUNDAI_KEY_WORD_DETECTOR_LIB_PATH' to specify the path.")
    endif()
    if(NOT SOUNDAI_KEY_WORD_DETECTOR_INCLUDE_DIR)
        message(FATAL_ERROR "Must pass include dir path of SoundAi KeywordDetector! You can set 'SOUNDAI_KEY_WORD_DETECTOR_INCLUDE_DIR' to specify the path.")
    endif()
    add_definitions(-DKWD)
    add_definitions(-DKWD_SOUNDAI)
    set(KWD ON)
endif()
