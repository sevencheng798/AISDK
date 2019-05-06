# Creater By Sven
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ASR LANGUAGES CXX)

add_subdirectory("src")

#if (ENABLE_SOUNDAI_ASR)

#elseif (ENABLE_IFLYTEK_AIUI_ASR)

#	add_subdirectory("AIUI")
#else ()
#message(FATAL_ERROR "You must set SOUNDAI_ASR to enable SAI SDK "
#		"or set IFLYTEK_AIUI_ASR to enable IflyTek AIUI SDK")
#endif()
