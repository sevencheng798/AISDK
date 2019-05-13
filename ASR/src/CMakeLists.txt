##
# Create By Sven
#
if (ENABLE_IFLYTEK_AIUI_ASR)
aux_source_directory(${ASR_SOURCE_DIR}/AIUI/src asr_SOURCES)
endif ()

if (ENABLE_SOUNDAI_ASR)
aux_source_directory(${ASR_SOURCE_DIR}/SoundAi/src asr_SOURCES)
endif ()

add_library(ASR SHARED
    GenericAutomaticSpeechRecognizer.cpp
    AutomaticSpeechRecognizerRegister.cpp
	MessageConsumer.cpp
    ASRTimer.cpp
	${asr_SOURCES})

include_directories(ASR 
	"${ASR_SOURCE_DIR}/include"
	"${JSONCPP_INCLUDE_DIR}")

if (ENABLE_IFLYTEK_AIUI_ASR)
include_directories(ASR 
	"${ASR_SOURCE_DIR}/AIUI/include"
	"${IFLYTEK_AIUI_ASR_INCLUDE_DIR}")
endif ()

if(ENABLE_SOUNDAI_ASR)
include_directories(ASR 
	"${ASR_SOURCE_DIR}/SoundAi/include"
	"${SOUNDAI_ASR_INCLUDE_DIR}")
endif()

target_link_libraries(ASR AICommon)

# install target
asdk_install()
