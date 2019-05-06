# Creater By Sven

add_library(AudioMediaPlayer SHARED
	AOEngine.cpp
	AOWrapper.cpp
	FFmpegDecoder.cpp
	FFmpegDeleter.cpp
	FFmpegUrlInputController.cpp
	FFmpegStreamInputController.cpp
	FFmpegAttachmentInputController.cpp
	PlaybackConfiguration.cpp
	RetryTimer.cpp)

target_include_directories(AudioMediaPlayer PUBLIC
	"${AudioMediaPlayer_SOURCE_DIR}/include"
	"${FFMPEG_INCLUDE_DIR}"
	"${OPENSSL_INCLUDE_DIR}"
	"${AO_INCLUDE_DIR}")

target_link_libraries(AudioMediaPlayer
	AICommon
	# FFmpeg libraries
	avformat
	avcodec
	avdevice
	avfilter
	avutil
	swresample
	swscale
	ssl
	crypto)
	
asdk_install()

