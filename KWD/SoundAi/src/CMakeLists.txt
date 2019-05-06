# Creater By Sven
add_library(KeywordDetector SHARED
			KeywordDetector.cpp)

target_include_directories(KeywordDetector PUBLIC
        "${KeywordDetector_SOURCE_DIR}/include"
		"${SoundAi_SOURCE_DIR}/include")
		
target_link_libraries(KeywordDetector
		AICommon)

asdk_install()

