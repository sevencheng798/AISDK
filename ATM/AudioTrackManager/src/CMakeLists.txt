#
#Creator by Sven
#
add_library(AudioTrackManager SHARED
    Channel.cpp
    AudioTrackManager.cpp)

include_directories(AudioTrackManager "${AudioTrackManager_SOURCE_DIR}/include")
target_link_libraries(AudioTrackManager AICommon)

# install target
asdk_install()
