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
pkg_search_module(gstreamer-rtsp REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0>=1.4)

add_executable(test-mp4 ./test-mp4.c)
target_link_libraries(test-mp4 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video PkgConfig::gstreamer-rtsp)

add_executable(test-ogg ./test-ogg.c)
target_link_libraries(test-ogg PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video PkgConfig::gstreamer-rtsp)

add_executable(test-readme ./test-readme.c)
target_link_libraries(test-readme PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video PkgConfig::gstreamer-rtsp)
