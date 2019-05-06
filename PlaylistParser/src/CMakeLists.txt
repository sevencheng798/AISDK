
add_library(PlaylistParser SHARED
    IterativePlaylistParser.cpp
    PlaylistUtils.cpp)

target_include_directories(PlaylistParser PUBLIC
    "${PlaylistParser_SOURCE_DIR}/include")

target_link_libraries(PlaylistParser AICommon)

# install target
asdk_install()
