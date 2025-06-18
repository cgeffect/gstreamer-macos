
cmake_minimum_required(VERSION 3.22.2)

project(allTarget)
set(CMAKE_CXX_STANDARD 17)
set(APP_NAME player)
set(CMAKE_SYSTEM_PROCESSOR "arm64")
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "11" CACHE STRING "Minimum OS X deployment version")

find_package(PkgConfig REQUIRED)
pkg_check_modules(gtk3 REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)
pkg_search_module(gstreamer-audio REQUIRED IMPORTED_TARGET gstreamer-audio-1.0>=1.4)
pkg_search_module(gstreamer-pbutils REQUIRED IMPORTED_TARGET gstreamer-pbutils-1.0>=1.4)
# pkg_search_module(gstreamer-rtsp REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0>=1.4)

# Automoc configuration for Qt6, 不加会导致如果加了Q_OBJECT宏值导致虚函数未实现的问题
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find the Qt6 package
find_package(Qt6 COMPONENTS Core Gui OpenGLWidgets Network REQUIRED)

list(APPEND LEAP_INCLUDES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Include the Qt6 directories
include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${Qt6OpenGLWidgets_INCLUDE_DIRS} ${Qt6Network_INCLUDE_DIRS})

file(GLOB_RECURSE leap_files
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/view/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/view/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/decode/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/decode/*.cpp
)
if (APPLE) 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${leap_files})
    set_source_files_properties(${leap_files} PROPERTIES COMPILE_FLAGS "${ALL_WARN_FLAG}")
endif()

# Add the source files
add_executable(${APP_NAME} ${leap_files} ${FILTERED_SOURCE_FILES})
set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
target_include_directories(${APP_NAME} PUBLIC ${SRV_INCLUDES_DIR} ${LEAP_INCLUDES_DIR})

# Link the Qt6 libraries
target_link_libraries(${APP_NAME} 
PRIVATE Qt6::Core Qt6::Gui Qt6::OpenGLWidgets Qt6::Network ${SRV_LIBRARY_LIBS} dl z 
PkgConfig::gtk3 PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)
#########################################################################################################################
