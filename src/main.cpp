#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include "error.hpp"
#include "mesh.hpp"
#include "config.h"
#include "glutils.hpp"
#include "shader.hpp"

glm::vec3 getArcBall(const glm::vec2 pos, int width, int height)
{
    glm::vec3 p =
        glm::vec3((2.0f * pos.x) / static_cast<float>(width) - 1.0f,
                  (2.0f * pos.y) / static_cast<float>(height) - 1.0f, 0);
    p.y = -p.y;
    float op = p.x * p.x + p.y * p.y;
    if (op <= 1) {
        p.z = static_cast<float>(sqrt(1.0 - op));
    } else {
        p = glm::normalize(p);
    }
    return p;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Setup windows and create context
    int width = 800;
    int height = 600;
    int delta = 5;
    

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Debug;
    sf::Window window(sf::VideoMode(width, height), "OpenGL", sf::Style::Default,
                      settings);
    window.setVerticalSyncEnabled(true);

    // init glew to wrangle gl pointers
    glrfw::init_gl();

    // enable debugging
    glDebugMessageCallback(&glrfw::debug_callback,nullptr);
    // print context version
    std::cout << glrfw::glsl_version() << std::endl;
    std::cout << glrfw::gl_version_string() << std::endl;

    // load mesh
    glrfw::mesh mesh =
        glrfw::parse_stl(glrfw::resource_path + std::string("kiefer.stl"));
    std::cout << mesh.vertices.size() << std::endl;

    // load vertex and fragment shader
    glrfw::shader vertex(glrfw::shader_type::vertex,
                         glrfw::resource_path + std::string("pixel.vert"));
    
    glrfw::shader fragment(glrfw::shader_type::fragment,
                           glrfw::resource_path + std::string("pixel.frag"));

    glrfw::shader vertex_point(glrfw::shader_type::vertex,
                               glrfw::resource_path + std::string("point.vert"));
    glrfw::shader fragment_point(glrfw::shader_type::fragment,
                                 glrfw::resource_path +
                                     std::string("point.frag"));
    glrfw::program program_point(std::move(vertex_point),
                                 std::move(fragment_point));

    // create program
    glrfw::program program(std::move(vertex),std::move(fragment));
    // create matrices
    auto projection = glm::perspective(45.0f, static_cast<float>(width) /
                                                  static_cast<float>(height),
                                       0.1f, 1000.0f);
    auto model =
        glm::lookAt(glm::vec3(0.0f, 0.0f, delta), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    auto normal = glm::transpose(glm::inverse(glm::mat3(model)));

    program.set_attribute(0, "in_Position");
    program.set_attribute(1, "in_Normals");
    program.link();
    program.bind();
    program.set_uniform("projectionMatrix", projection);
    program.set_uniform("modelviewMatrix", model);
    program.set_uniform("normalMatrix", normal);
    std::cout << program.attributes() << std::endl;
    std::cout << program.uniforms() << std::endl;

    program_point.set_attribute(0, "in_Position");
    program_point.link();
    program_point.bind();
    program_point.set_uniform("projectionMatrix", projection);
    program_point.set_uniform("modelviewMatrix", model);
    std::cout << program_point.attributes() << std::endl;
    std::cout << program_point.uniforms() << std::endl;
    
    glViewport(0,0,width,height);
    glEnable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
    glPointSize(10.0f);

    GLuint vbos[4];
    glGenBuffers(4,&vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3),
                 &mesh.vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh.triangles.size() * sizeof(glm::ivec3),
                 &mesh.triangles[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_normals.size() * sizeof(glm::vec3),
                 &mesh.vertex_normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    GLuint vao[2];
    glGenVertexArrays(2, &vao[0]);
    glBindVertexArray(vao[0]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[2]);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbos[1]);
    
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0); 
    
    bool running = true;
    bool mouse_pressed = false;

    glm::vec2 start_pos;
    glm::vec2 cur_pos;
    glm::vec3 light_pos(0,0,100);
    float angle = 0.0f;
    bool move_light = false;

    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), &light_pos);

    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            } else if (event.type == sf::Event::Resized) {
                width = event.size.width;
                height = event.size.height;
                glViewport(0, 0, width, height);
                projection =
                    glm::perspective(45.0f, static_cast<float>(width) /
                                                static_cast<float>(height),
                                     0.1f, 1000.0f);
            } else if (event.type == sf::Event::MouseWheelMoved) {
                delta += event.mouseWheel.delta;
                glm::mat3 eye = glm::inverse(glm::mat3(model));
                glm::vec3 tran = eye * glm::vec3(0.0f,0.0f,delta);
                model = glm::translate(model,tran);
                normal = glm::transpose(glm::inverse(glm::mat3(model)));
                delta = 0;
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mouse_pressed = true;
                    start_pos.x = static_cast<float>(event.mouseButton.x);
                    start_pos.y = static_cast<float>(event.mouseButton.y);
                    cur_pos.x = static_cast<float>(event.mouseButton.x);
                    cur_pos.y = static_cast<float>(event.mouseButton.y);
                } else if (event.mouseButton.button = sf::Mouse::Right) {
                    move_light = true;
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                mouse_pressed = false;
                move_light = false;
            } else if (event.type == sf::Event::MouseMoved) {
                if (mouse_pressed) {
                    cur_pos.x = static_cast<float>(event.mouseMove.x);
                    cur_pos.y = static_cast<float>(event.mouseMove.y);
                }
            }
        }

        if (mouse_pressed) {
            if( cur_pos != start_pos) {
                glm::vec3 va = getArcBall(start_pos, width, height);
                glm::vec3 vb = getArcBall(cur_pos, width, height);
                float angle = acos(std::min(1.0f, glm::dot(va, vb)));
                glm::vec3 axis = glm::cross(va, vb);
                glm::mat3 eye = glm::inverse(glm::mat3(model));
                glm::vec3 obj_axis = eye * axis;
                model = glm::rotate(model, 0.1f * glm::degrees(angle), obj_axis);
                normal = glm::transpose(glm::inverse(glm::mat3(model)));
                start_pos = cur_pos;
            }
        }

        if ( move_light) {
            angle += 10;
            if (angle > 360)
                angle = 0.0;
            light_pos.x = 100.0f * cos(glm::radians(angle));
            light_pos.y = 100.0f * sin(glm::radians(angle));

            glBindVertexArray(vao[1]);
            glBindBuffer(GL_ARRAY_BUFFER,vbos[3]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3),&light_pos);

        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao[0]);
        program.bind();
        program.set_uniform("projectionMatrix", projection);
        program.set_uniform("modelviewMatrix", model);
        program.set_uniform("normalMatrix", normal);
        program.set_uniform("lightpos",light_pos);
        glDrawElements(GL_TRIANGLES, mesh.triangles.size() * 3, GL_UNSIGNED_INT,
                       nullptr);
        glBindVertexArray(vao[1]);
        program_point.bind();
        program_point.set_uniform("projectionMatrix", projection);
        program_point.set_uniform("modelviewMatrix", model);
        glDrawArrays(GL_POINTS,0,1);
        window.display();
    }
    return 0;
}
