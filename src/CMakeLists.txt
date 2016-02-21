cmake_minimum_required(VERSION 3.0)
project(glrfw)

set (src
   main.cpp
   mesh.cpp)


set(CMAKE_CXX_FLAGS "-std=c++14")


find_package(SFML 2 REQUIRED system window)

message(${SFML_INCLUDE_DIR})
include_directories(${SFML_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extern/glm-0.9.7.2/)


add_executable(glrfw ${src})
target_link_libraries(glrfw ${SFML_LIBRARIES} GL pthread)


