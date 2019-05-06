#
# Creater By Sven
#
add_library(SoundAi SHARED
			SoundAiEngine.cpp
			MessageConsumer.cpp)

target_include_directories(SoundAi PUBLIC
        "${SoundAi_SOURCE_DIR}/include")

target_link_libraries(SoundAi
        sai_sdk
		AICommon
		KeywordDetector)

# Install sai sdk library
file(GLOB SAI_SDK_LIBRARIES ${SoundAi_SOURCE_DIR}/lib/*)
install(PROGRAMS ${SAI_SDK_LIBRARIES} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
	
asdk_install()

