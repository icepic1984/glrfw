cmake_minimum_required(VERSION 3.0)

project(glrfw)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Weffc++ -pedantic  \
    -pedantic-errors -Wextra  -Wcast-align \
    -Wcast-qual  -Wchar-subscripts  -Wcomment -Wconversion \
    -Wdisabled-optimization \
    -Wfloat-equal  -Wformat  -Wformat=2 \
    -Wformat-nonliteral -Wformat-security  \
    -Wformat-y2k \
    -Wimport  -Winit-self  -Winline \
    -Winvalid-pch   \
    -Wunsafe-loop-optimizations  -Wlong-long -Wmissing-braces \
    -Wmissing-field-initializers -Wmissing-format-attribute   \
    -Wmissing-include-dirs -Wmissing-noreturn \
    -Wpacked  -Wparentheses  -Wpointer-arith \
    -Wredundant-decls -Wreturn-type \
    -Wsequence-point  -Wshadow -Wsign-compare  -Wstack-protector \
    -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch  -Wswitch-default \
    -Wswitch-enum -Wtrigraphs  -Wuninitialized \
    -Wunknown-pragmas  -Wunreachable-code -Wunused \
    -Wunused-function  -Wunused-label  -Wunused-parameter \
    -Wunused-value  -Wunused-variable  -Wvariadic-macros \
    -Wvolatile-register-var  -Wwrite-strings")

find_package(SFML 2 REQUIRED system window)
find_package(GLEW REQUIRED)
find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${SFML_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/glm-0.9.7.2/)
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})

message(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test (NAME test_shader COMMAND shader)
