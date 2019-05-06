#
# Creator By Sven
#
cmake_minimum_required(VERSION 3.1)

set(SoundAiEngineTest_SOURCES)
list(APPEND SoundAiEngineTest_SOURCES SoundAiEngineTest.cpp)

add_executable(SoundAiEngineTest ${SoundAiEngineTest_SOURCES})

target_include_directories(SoundAiEngineTest PUBLIC
		"${SoundAi_SOURCE_DIR}/include"
		"${AudioTrackManager_SOURCE_DIR}/include")

target_link_libraries(SoundAiEngineTest
		SoundAi
		NLP
		AudioTrackManager
		asound
		z)

install(TARGETS SoundAiEngineTest
      RUNTIME DESTINATION bin
      BUNDLE  DESTINATION bin
      LIBRARY DESTINATION lib)
