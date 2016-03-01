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

    int width = 800;
    int height = 600;
    
    glrfw::mesh mesh = glrfw::parse_stl("D:\\Meshes\\monkey.stl");

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 5;
    settings.attributeFlags = sf::ContextSettings::Debug;
    sf::Window window(sf::VideoMode(width, height), "OpenGL", sf::Style::Default,
                      settings);
    window.setVerticalSyncEnabled(true);

    glrfw::init_gl();
    // enable debugging
    glDebugMessageCallback(&glrfw::debug_callback,nullptr);
    std::cout << glrfw::glsl_version() << std::endl;
    std::cout << glrfw::gl_version_string() << std::endl;

    glrfw::shader vertex(glrfw::shader_type::vertex,
                         "D:\\Projects\\glrfw\\resources\\pixel.vert");

    glrfw::shader fragment(glrfw::shader_type::fragment,
                           "D:\\Projects\\glrfw\\resources\\pixel.frag");

    glrfw::program program(std::move(vertex),std::move(fragment));
    auto projection = glm::perspective(45.0f, static_cast<float>(width) /
                                                  static_cast<float>(height),
                                       0.1f, 1000.0f);
    auto model =
        glm::lookAt(glm::vec3(0.0f, 0.0f, 100), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    auto normal = glm::transpose(glm::inverse(glm::mat3(model)));


    program.link();
    program.bind();
    program.set_uniform("projectionMatrix", projection);
    program.set_uniform("modelviewMatrix", model);
    program.set_uniform("normalMatrix", normal);
    std::cout << program.attributes() << std::endl;
    std::cout << program.uniforms() << std::endl;
    // glViewport(0,0,width,height);
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_TEXTURE_2D);
	// glEnable (GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glClearColor(0.0f,0.0f,0.0f,1.0f);

    // auto projection = glm::perspective(45.0f, static_cast<float>(width) /
    //                                               static_cast<float>(height),
    //                                   0.1f, 1000.0f);

    // auto model =
    //     glm::lookAt(glm::vec3(0.0f, 0.0f, 100), glm::vec3(0.0f, 0.0f, 0.0f),
    //                 glm::vec3(0.0f, 1.0f, 0.0f));
    // auto normal  = glm::transpose(glm::inverse(glm::mat3(model)));
    // program.bind();
    // program.set_uniform("projectionMatrix", projection);
    // program.set_uniform("modelviewMatrix", model);
    // program.set_uniform("normalMatrix", normal);
    // std::cout << program.uniforms() << std::endl;

    // GLuint vao;
    // glGenVertexArrays(1, &vao);
    // GLuint vbos[3];
    // glBindVertexArray(vao);
    // glGenBuffers(3,&vbos[0]);
    // glEnableVertexAttribArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    // glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3),
    //              &mesh.vertices[0],GL_STATIC_DRAW);
    // glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //              mesh.triangles.size() * sizeof(glm::ivec3), &mesh.triangles[0],
    //              GL_STATIC_DRAW);
    // glEnableVertexAttribArray(1);
    // glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    // glBufferData(GL_ARRAY_BUFFER,
    //              mesh.vertex_normals.size() * sizeof(glm::vec3),
    //              &mesh.vertex_normals[0], GL_STATIC_DRAW);
    // glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0); 
    

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
