cmake_minimum_required(VERSION 3.14)
project(Skagway2)

set(CMAKE_CXX_STANDARD 17)

find_library(libuvc REQUIRED)
find_library(folly REQUIRED)
find_library(glog REQUIRED)

set(CAMERA_SOURCE Camera.cpp Camera.h)
set(FRAME_WRITER_SOURCE FrameWriter.cpp FrameWriter.h)

set(LIBUVC uvc usb-1.0)

add_library(camera ${CAMERA_SOURCE})
target_link_libraries(camera folly)

add_library(frame_writer ${FRAME_WRITER_SOURCE})
target_link_libraries(frame_writer camera stdc++fs)

add_executable(Skagway2 main2.cpp)

add_executable(test main.cpp)
target_link_libraries(Skagway2 ${LIBUVC} camera frame_writer glog pthread)