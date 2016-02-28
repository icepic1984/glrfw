#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include "error.hpp"
#include "mesh.hpp"

#include "glutils.hpp"
#include "shader.hpp"

// int main(int argc, char *argv[])
// {

// 	static_assert(glrfw::has_glm_to_string<glm::vec3>() == true,"bla");
// 	//static_assert(glrfw::has_glm_to_string<int>::value == true,"bla");

// 	glrfw::mesh mesh;
// 	mesh.add_triangle(glm::vec3(0,0,0),glm::vec3(1,0,0),glm::vec3(1,1,0));
// 	mesh.add_triangle(glm::vec3(1,0,0),glm::vec3(2,1,0),glm::vec3(1,1,0));

// 	mesh.print_vertices();
// 	std::cout << "****" << std::endl;
// 	mesh.print_triangles();
// 	mesh.print_neighbors();

// 	return 0;
// }

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    glrfw::mesh mesh = glrfw::parse_stl("D:\\Meshes\\monkey.stl");

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 5;
    settings.attributeFlags = sf::ContextSettings::Core;
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default,
                      settings);
    window.setVerticalSyncEnabled(true);

    glrfw::init_gl();
    std::cout << glrfw::glsl_version() << std::endl;
    std::cout << glrfw::gl_version_string() << std::endl;

    glrfw::shader shader(glrfw::shader_type::vertex, "D:\\Meshes\\test.vert");
    glrfw::program program(std::move(shader));
    program.link();

    bool running = true;
    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window.display();
    }
    return 0;
}
