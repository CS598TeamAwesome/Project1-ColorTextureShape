cmake_minimum_required (VERSION 2.6)

project(ColorTextureShape)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)

add_executable(OpenCVTest OpenCVTest.cpp)
target_link_libraries(OpenCVTest ${OpenCV_LIBS})

