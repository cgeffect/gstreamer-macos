# https://cloud.tencent.com/developer/ask/sof/107978377
cmake_minimum_required(VERSION 3.15.3)
project(gstreamer)

set(CMAKE_C_FLAGS_DEBUG   "-O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_SYSTEM_PROCESSOR "arm64")
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "14" CACHE STRING "Minimum OS X deployment version")

# 系统路径查找 设置qt的软链
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_PREFIX_PATH /opt/homebrew/)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(gtk3 REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)
pkg_search_module(gstreamer-audio REQUIRED IMPORTED_TARGET gstreamer-audio-1.0>=1.4)
pkg_search_module(gstreamer-pbutils REQUIRED IMPORTED_TARGET gstreamer-pbutils-1.0>=1.4)

add_executable(tutorial1 ./src/basic-tutorial-1.c)
target_link_libraries(tutorial1 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(tutorial2 ./src/basic-tutorial-2.c)
target_link_libraries(tutorial2 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(tutorial3 ./src/basic-tutorial-3.c)
target_link_libraries(tutorial3 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(tutorial4 ./src/basic-tutorial-4.c)
target_link_libraries(tutorial4 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

# add_executable(tutorial5 ./src/basic-tutorial-5.c)
# target_link_libraries(tutorial5 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(tutorial6 ./src/basic-tutorial-6.c)
target_link_libraries(tutorial6 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(tutorial7 ./src/basic-tutorial-7.c)
target_link_libraries(tutorial7 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(tutorial8 ./src/basic-tutorial-8.c)
target_link_libraries(tutorial8 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video PkgConfig::gstreamer-audio)

add_executable(tutorial9 ./src/basic-tutorial-9.c)
target_link_libraries(tutorial9 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video PkgConfig::gstreamer-pbutils) 

add_executable(tutorial12 ./src/basic-tutorial-12.c)
target_link_libraries(tutorial12 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(tutorial13 ./src/basic-tutorial-13.c)
target_link_libraries(tutorial13 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(playback1 ./src/playback-tutorial-1.c)
target_link_libraries(playback1 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(playback2 ./src/playback-tutorial-2.c)
target_link_libraries(playback2 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(playback3 ./src/playback-tutorial-3.c)
target_link_libraries(playback3 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video PkgConfig::gstreamer-audio)

add_executable(playback4 ./src/playback-tutorial-4.c)
target_link_libraries(playback4 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(playback5 ./src/playback-tutorial-5.c)
target_link_libraries(playback5 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(playback6 ./src/playback-tutorial-6.c)
target_link_libraries(playback6 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(playback7 ./src/playback-tutorial-7.c)
target_link_libraries(playback7 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(play-ogg ./src/play-ogg.c)
target_link_libraries(play-ogg PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_executable(gst-decoder ./src/gst-decoder.c)
target_link_libraries(gst-decoder PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

add_subdirectory(player)