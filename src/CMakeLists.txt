cmake_minimum_required(VERSION 3.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

project(glrfw)

set (src
   glutils.cpp
   shader.cpp
   mesh.cpp
   main.cpp
)


set(CMAKE_CXX_FLAGS "-std=c++14")


find_package(SFML 2 REQUIRED system window)
find_package(GLEW REQUIRED)

include_directories(${SFML_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extern/glm-0.9.7.2/)
include_directories(${GLEW_INCLUDE_DIRS})


add_executable(glrfw ${src})
target_link_libraries(glrfw ${SFML_LIBRARIES} ${GLEW_LIBRARIES} -lopengl32 pthread)


# cmake -G"MSYS Makefiles"  -DCMAKE_BUILD_TYPE=Release ../../src/
