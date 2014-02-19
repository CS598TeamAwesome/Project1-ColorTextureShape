cmake_minimum_required (VERSION 2.6)

project(ColorTextureShape)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE)
set(HEADERS)

add_subdirectory(Feature)

add_library(ColorTextureShape ${SOURCE} ${HEADERS})
target_link_libraries(ColorTextureShape ${OpenCV_LIBS})

