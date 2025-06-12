cmake_minimum_required(VERSION 3.15.3)
project(gstreamer)

set(CMAKE_C_FLAGS_DEBUG   "-O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

set(CMAKE_SYSTEM_PROCESSOR "x86_64")
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gtk3 REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)

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

# add_executable(tutorial8 ./src/basic-tutorial-8.c)
# target_link_libraries(tutorial8 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)

# add_executable(tutorial9 ./src/basic-tutorial-9.c)
# target_link_libraries(tutorial9 PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)
