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
	"${ASR_SOURCE_DIR}/include")

if (ENABLE_IFLYTEK_AIUI_ASR)
include_directories(ASR 
	"${ASR_SOURCE_DIR}/AIUI/include"
	"${CMAKE_SOURCE_DIR}/ThirdLibrary/Iflytek/include")
endif ()

if(ENABLE_SOUNDAI_ASR)
include_directories(ASR 
	"${ASR_SOURCE_DIR}/SoundAi/include")
endif()

target_link_libraries(ASR AICommon)

# install target
asdk_install()
