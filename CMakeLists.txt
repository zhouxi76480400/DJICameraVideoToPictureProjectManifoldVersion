cmake_minimum_required(VERSION 2.8)
project(DJICameraVideoToPictureProjectManifoldVersion)

add_compile_options(-std=c++11)
set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS -pthread)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(DJICameraVideoToPictureProjectManifoldVersion
        main.cpp
        video_to_picture/ConvertUtil.h
        video_to_picture/ConvertUtil.cpp
        video_to_picture/video_to_picture.h
        video_to_picture/video_to_picture.cpp
        )

target_link_libraries(DJICameraVideoToPictureProjectManifoldVersion
#        ${CMAKE_SOURCE_DIR}/lib/libdcam.so
        )