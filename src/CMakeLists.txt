set (src
   glutils.cpp
   shader.cpp
   mesh.cpp
)

add_library(libglrfw ${src})

add_executable(glrfw main.cpp)
target_link_libraries(glrfw libglrfw ${SFML_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY} pthread)


