cmake_minimum_required(VERSION 2.8)

set(SampleApp_SOURCES)
list(APPEND SampleApp_SOURCES 
	Main.cpp SampleApp.cpp UIManager.cpp AIClient.cpp
	PortAudioMicrophoneWrapper.cpp
	ControlActionManager.cpp
	KeywordObserver.cpp)

# For AM113 to set the path.
link_directories(${JSONCPP_LIB_PATH})

add_executable(SampleApp ${SampleApp_SOURCES})

target_include_directories(SampleApp PUBLIC
		"${SampleApp_SOURCE_DIR}/include"
		"${AudioTrackManager_SOURCE_DIR}/include"
		"${PORTAUDIO_INCLUDE_DIR}"
		"${KWD_SOURCE_DIR}/include"
		"${ASR_SOURCE_DIR}/include")

target_link_libraries(SampleApp
		AICommon
		AudioTrackManager
		AudioMediaPlayer
		NLP
		SpeechSynthesizer
		ASR
		KWD
		portaudio
		ao
		asound
		jsoncpp
		z)
if(SOUNDAI_ASR)
	target_link_libraries(SampleApp sai_sdk)
elseif(IFLYTEK_AIUI_ASR)
	target_link_libraries(SampleApp aiui)
endif()

if(SOUNDAI_KEY_WORD_DETECTOR)
	target_link_libraries(SampleApp sai_sdk)
elseif(IFLYTEK_KEY_WORD_DETECTOR)
	target_link_libraries(SampleApp msc)
endif()

install(TARGETS SampleApp
      RUNTIME DESTINATION bin
      BUNDLE  DESTINATION bin
      LIBRARY DESTINATION lib)
