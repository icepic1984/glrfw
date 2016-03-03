set (src
   glutils.cpp
   shader.cpp
   mesh.cpp
)

if (WIN32)
    set(RESOURCE_PATH "D:/Projects/glrfw/resources/")
else ()
    set(RESOURCE_PATH "/home/icepic/Code/glrfw/resources/")
endif()

configure_file(config.h.in ${CMAKE_CURRENT_LIST_DIR}/config.h)
add_library(libglrfw ${src})

add_library(lodepng lodepng.cpp)

add_executable(glrfw main.cpp)
target_link_libraries(glrfw libglrfw lodepng ${SFML_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY} pthread)



