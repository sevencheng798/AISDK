cmake_minimum_required(VERSION 3.0)

set(MicroPhoneTest_SOURCES)
list(APPEND MicroPhoneTest_SOURCES 
	MicroPhoneTest.cpp
	PortAudioMicrophoneWrapper.cpp)

add_executable(MicroPhoneTest ${MicroPhoneTest_SOURCES})

target_include_directories(MicroPhoneTest PUBLIC
		"${MicroPhoneTest_SOURCES_DIR}/include"
		"${PORTAUDIO_INCLUDE_DIR}"
		"../include")

target_link_libraries(MicroPhoneTest
		AICommon
		portaudio
		asound
		z)

install(TARGETS MicroPhoneTest
      RUNTIME DESTINATION bin
      BUNDLE  DESTINATION bin
      LIBRARY DESTINATION lib)
