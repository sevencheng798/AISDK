#
# Creator by Sven
#
add_library(KeywordDetector SHARED 
	IflyTekKeywordDetector.cpp)

target_include_directories(KeywordDetector PUBLIC
	"${KeywordDetector_SOURCE_DIR}/include"
	"${KWD_SOURCE_DIR}/include"
	"${IFLYTEK_KEY_WORD_DETECTOR_INCLUDE}")

target_link_libraries(KeywordDetector AICommon KWD)
	
# install target
asdk_install()
