include_directories(${glrfw_SOURCE_DIR}/src)
add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable(shader shader.cpp)
target_link_libraries(shader libglrfw ${Boost_LIBRARIES} ${SFML_LIBRARIES} ${GLEW_LIBRARIES} -lopengl32 pthread) 

