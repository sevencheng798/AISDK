# Create by Sven
# 201902

project(AICommon)

add_subdirectory("Utils")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Utils/src/Logging  Logging_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Utils/src/Attachment  Attachment_SOURCES)

add_library(AICommon SHARED 
	Utils/src/DeviceInfo.cpp
	Utils/src/Executor.cpp
	Utils/src/TaskQueue.cpp
	Utils/src/TaskThread.cpp
	Utils/src/DialogRelay/DialogUXStateRelay.cpp
	Utils/src/SafeShutdown.cpp
	Utils/src/SharedBuffer/BufferLayout.cpp
	Utils/src/SharedBuffer/SharedBuffer.cpp
	Utils/src/SharedBuffer/Reader.cpp
	Utils/src/SharedBuffer/Writer.cpp
	${Attachment_SOURCES}
	${Logging_SOURCES})

target_include_directories(AICommon PUBLIC 
	"${AICommon_SOURCE_DIR}/Utils/include"
	"${AICommon_SOURCE_DIR}/DMInterface/include"
	"${CURL_INCLUDE_DIRS}")

target_link_libraries(AICommon
    ${CURL_LIBRARIES})

LIST(APPEND PATHS 
	"${PROJECT_SOURCE_DIR}/Utils/include"
	"${AICommon_SOURCE_DIR}/DMInterface/include")

asdk_install_multiple("${PATHS}")

