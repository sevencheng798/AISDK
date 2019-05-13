# Creater By Sven
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

LIST(APPEND 3RD_LIBS "${ThirdLibrary_SOURCE_DIR}/Iflytek/lib/libaiui.so")
LIST(APPEND 3RD_LIBS "${ThirdLibrary_SOURCE_DIR}/Iflytek/lib/libmsc.so")
install(PROGRAMS  ${3RD_LIBS} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
