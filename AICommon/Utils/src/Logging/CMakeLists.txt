# Creater By Sven
project(Logger LANGUAGES CXX)

add_library(Logger SHARED
        LogEntry.cpp
        LogEntryBuffer.cpp
        LogEntryStream.cpp
		ConsoleLogger.cpp
		Level.cpp
		Logger.cpp
		LogStringFormatter.cpp
		ThreadMoniker.cpp
		SafeCTimeAccess.cpp)


target_include_directories(Logger PUBLIC
        "${Logger_SOURCE_DIR}/../include")

#target_link_libraries(Logger)
	
#asdk_install()

