##
# Create By Sven
#

if (IFLYTEK_KEY_WORD_DETECTOR)
aux_source_directory(${KWD_SOURCE_DIR}/IflyTek/src/ Keyword_SOURCES)
endif ()

if (SOUNDAI_KEY_WORD_DETECTOR)
aux_source_directory(${KWD_SOURCE_DIR}/SoundAi/src/ Keyword_SOURCES)
endif ()

add_library(KWD SHARED
    GenericKeywordDetector.cpp
    KeywordDetectorRegister.cpp
	${Keyword_SOURCES})

include_directories(KWD 
	"${KWD_SOURCE_DIR}/include")

if (IFLYTEK_KEY_WORD_DETECTOR)
include_directories(KWD 
	"${KWD_SOURCE_DIR}/IflyTek/include"
	"${IFLYTEK_KEY_WORD_DETECTOR_INCLUDE_DIR}")
endif ()

if(SOUNDAI_KEY_WORD_DETECTOR)
include_directories(KWD 
	"${SOUNDAI_KEY_WORD_DETECTOR_DIR}")
endif()

target_link_libraries(KWD AICommon)

# install target
asdk_install()