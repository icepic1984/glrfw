#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include "error.hpp"
#include "mesh.hpp"
#include "config.h"
#include "glutils.hpp"
#include "shader.hpp"
#include "lodepng.h"
#include "error.hpp"

std::vector<glm::vec3> quad 
{
    {-1.0f,-1.0f,0.0f},
    {-0.5f,-1.0f,0.0f},
    {-1.0f,-0.5f,0.0f},
    {-1.0f,-0.5f,0.0f},
    {-0.5f,-1.0f,0.0f},
    {-0.5f,-0.5f,0.0f}
};

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
    glm::ivec2 viewport_size(800,600);
    glm::ivec2 depthmap_size(2048,2048);
    int delta = 5;
    
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 0;
    settings.attributeFlags = sf::ContextSettings::Debug;
    sf::Window window(sf::VideoMode(viewport_size.x, viewport_size.y), "OpenGL",
                      sf::Style::Default, settings);
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

    // load vertex and fragment shader
    glrfw::shader vertex(glrfw::shader_type::vertex,
                         glrfw::resource_path + std::string("pixel.vert"));
    glrfw::shader fragment(glrfw::shader_type::fragment,
                           glrfw::resource_path + std::string("pixel.frag"));
    glrfw::program program(std::move(vertex),std::move(fragment));

    glrfw::shader vertex_point(glrfw::shader_type::vertex,
                               glrfw::resource_path + std::string("point.vert"));
    glrfw::shader fragment_point(glrfw::shader_type::fragment,
                                 glrfw::resource_path +
                                     std::string("point.frag"));
    glrfw::program program_point(std::move(vertex_point),
                                 std::move(fragment_point));

    glrfw::shader vertex_depth(glrfw::shader_type::vertex,
                               glrfw::resource_path + std::string("depth.vert"));
    glrfw::shader fragment_depth(glrfw::shader_type::fragment,
                                 glrfw::resource_path +
                                     std::string("depth.frag"));
    glrfw::program program_depth(std::move(vertex_depth),
                                 std::move(fragment_depth));

    glrfw::shader vertex_shadow(glrfw::shader_type::vertex,
                               glrfw::resource_path + std::string("shadow.vert"));
    glrfw::shader fragment_shadow(glrfw::shader_type::fragment,
                                  glrfw::resource_path +
                                      std::string("shadow.frag"));
    glrfw::program program_shadow(std::move(vertex_shadow),
                                  std::move(fragment_shadow));

    glrfw::shader vertex_quad(glrfw::shader_type::vertex,
                               glrfw::resource_path + std::string("quad.vert"));
    glrfw::shader fragment_quad(glrfw::shader_type::fragment,
                                 glrfw::resource_path +
                                     std::string("quad.frag"));
    glrfw::program program_quad(std::move(vertex_quad),
                                 std::move(fragment_quad));

    // create matrices
    auto projection =
        glm::perspective(45.0f, static_cast<float>(viewport_size.x) /
                                    static_cast<float>(viewport_size.y),
                         0.1f, 1000.0f);
    auto model =
        glm::lookAt(glm::vec3(0.0f, 0.0f, delta), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    auto normal = glm::transpose(glm::inverse(glm::mat3(model)));


    std::cout << "Program: " << std::endl;
    program.set_attribute(0, "in_Position");
    program.set_attribute(1, "in_Normals");
    program.link();
    program.bind();
    program.set_uniform("projectionMatrix", projection);
    program.set_uniform("modelviewMatrix", model);
    program.set_uniform("normalMatrix", normal);
    std::cout << program.attributes() << std::endl;
    std::cout << program.uniforms() << std::endl;
    program.unbind();

    std::cout << "Depth Program: " << std::endl;
    program_depth.set_attribute(0, "in_Position");
    program_depth.link();
    program_depth.bind();
    program_depth.set_uniform("projectionMatrix", projection);
    program_depth.set_uniform("modelviewMatrix", model);
    std::cout << program_depth.attributes() << std::endl;
    std::cout << program_depth.uniforms() << std::endl;
    program_depth.unbind();

    std::cout << "Quad Program: " << std::endl;
    program_quad.set_attribute(0, "in_Position");
    program_quad.link();
    program_quad.bind();
    std::cout << program_quad.attributes() << std::endl;
    std::cout << program_quad.uniforms() << std::endl;
    program_quad.unbind();

    std::cout << "Shadow Program: " << std::endl;
    program_shadow.set_attribute(0, "in_Position");
    program_shadow.set_attribute(1, "in_Normals");
    program_shadow.link();
    program_shadow.bind();
    std::cout << program_shadow.attributes() << std::endl;
    std::cout << program_shadow.uniforms() << std::endl;
    program_shadow.unbind();

    std::cout << "Point Program: " << std::endl;
    program_point.set_attribute(0, "in_Position");
    program_point.link();
    program_point.bind();
    program_point.set_uniform("projectionMatrix", projection);
    program_point.set_uniform("modelviewMatrix", model);
    std::cout << program_point.attributes() << std::endl;
    std::cout << program_point.uniforms() << std::endl;
    program_point.unbind();
    
    // Set up view port
    glViewport(0,0,viewport_size.x,viewport_size.y);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	glClearColor(1.0f,1.0f,1.0f,1.0f);
    glPointSize(10.0f);

    // Create framebuffer object
    GLuint fbo[2];
    glGenFramebuffers(2, &fbo[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);

    // Create texture object
    GLfloat border[] = {1.0f, 0.0f, 0.0f, 0.0f};
    GLuint depth_tex[2];
    glGenTextures(2, &depth_tex[0]);
    glBindTexture(GL_TEXTURE_2D, depth_tex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthmap_size.x,
                 depthmap_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depth_tex[0], 0);
    GLenum draw_buffers[] = {GL_NONE};
    glDrawBuffers(1, draw_buffers);
    THROW_IF(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE,
             glrfw::error_type::uniform_not_found);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
    glBindTexture(GL_TEXTURE_2D, depth_tex[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthmap_size.x,
                 depthmap_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depth_tex[1], 0);
    glDrawBuffers(1, draw_buffers);
    THROW_IF(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE,
             glrfw::error_type::uniform_not_found);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // Generate vertex buffer ojects
    GLuint vbos[5];
    glGenBuffers(5,&vbos[0]);
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
    glBindBuffer(GL_ARRAY_BUFFER,vbos[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * quad.size(), &quad[0],
                 GL_STATIC_DRAW);

    // Generate vertex array objects and bind mesh vbos to the current
    // vao
    GLuint vao[3];
    glGenVertexArrays(3, &vao[0]);

    // Bind vbo for rendering mesh to vao
    glBindVertexArray(vao[0]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[2]);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbos[1]);
    
    // Bind vbo for rendering of point light to vao
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0); 

    // Bind vao for rendering quad
    glBindVertexArray(vao[2]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[4]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    bool running = true;
    bool mouse_pressed = false;

    glm::vec2 start_pos;
    glm::vec2 cur_pos;
    glm::vec3 light_pos(0,0,0);
    float rotation_angle = 0.0f;
    bool move_light = false;

    glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5,
                         0.0, 0.5, 0.5, 0.5, 1.0);

    auto depth_projection =
        glm::perspective(45.0f, static_cast<float>(depthmap_size.x) /
                                    static_cast<float>(depthmap_size.y),
                         0.1f, 1000.0f);
    auto depth_view =
        glm::lookAt(light_pos, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    
    auto shadow_matrix = biasMatrix * depth_projection * depth_view;

    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), &light_pos);

    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            } else if (event.type == sf::Event::Resized) {
                viewport_size.x = event.size.width;
                viewport_size.y = event.size.height;
                glViewport(0, 0, viewport_size.x, viewport_size.y);
                projection = glm::perspective(
                    45.0f, static_cast<float>(viewport_size.x) /
                               static_cast<float>(viewport_size.y),
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
                } else if (event.mouseButton.button == sf::Mouse::Right) {
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
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P) {
                    // capture 
                }
            }
        }

        if (mouse_pressed) {
            if( cur_pos != start_pos) {
                glm::vec3 va =
                    getArcBall(start_pos, viewport_size.x, viewport_size.y);
                glm::vec3 vb = getArcBall(cur_pos, viewport_size.x, viewport_size.y);
                float angle = static_cast<float>(
                    std::acos(std::min(1.0f, glm::dot(va, vb))));
                glm::vec3 axis = glm::cross(va, vb);
                glm::mat3 eye = glm::inverse(glm::mat3(model));
                glm::vec3 obj_axis = eye * axis;
                model = glm::rotate(model, 0.1f * glm::degrees(angle), obj_axis);
                normal = glm::transpose(glm::inverse(glm::mat3(model)));
                start_pos = cur_pos;
            }
        }

        if ( move_light) {
            rotation_angle += 5;
            if (rotation_angle > 360)
                rotation_angle = 0.0;
            light_pos.x = 100.0f * static_cast<float>(
                                       std::cos(glm::radians(rotation_angle)));
            light_pos.y = 100.0f * static_cast<float>(
                                       std::sin(glm::radians(rotation_angle)));

            glBindVertexArray(vao[1]);
            glBindBuffer(GL_ARRAY_BUFFER,vbos[3]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3),&light_pos);

        }

        depth_projection =
        glm::perspective(45.0f, static_cast<float>(depthmap_size.x) /
                                    static_cast<float>(depthmap_size.y),
                         0.1f, 1000.0f);
        depth_view =
            glm::lookAt(light_pos, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));

        shadow_matrix = biasMatrix * depth_projection * depth_view;

        glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
        glViewport(0,0,depthmap_size.x,depthmap_size.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glBindVertexArray(vao[0]);
        program_depth.bind();
        program_depth.set_uniform("projectionMatrix", depth_projection);
        program_depth.set_uniform("modelviewMatrix", depth_view);
        glDrawElements(GL_TRIANGLES, mesh.triangles.size() * 3,
                        GL_UNSIGNED_INT, nullptr);
        program_depth.unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
        glViewport(0,0,depthmap_size.x,depthmap_size.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glBindVertexArray(vao[0]);
        program_depth.bind();
        program_depth.set_uniform("projectionMatrix", depth_projection);
        program_depth.set_uniform("modelviewMatrix", depth_view);
        glDrawElements(GL_TRIANGLES, mesh.triangles.size() * 3,
                        GL_UNSIGNED_INT, nullptr);
        program_depth.unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, viewport_size.x, viewport_size.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);

        //Render depth map to screen
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth_tex[0]);
        glBindVertexArray(vao[0]);
        program_shadow.bind();
        program_shadow.set_uniform("projectionMatrix", projection);
        program_shadow.set_uniform("modelviewMatrix", model);
        program_shadow.set_uniform("normalMatrix", normal);
        program_shadow.set_uniform("lightpos",light_pos);
        program_shadow.set_uniform("shadowMatrix",shadow_matrix);
        int loc = glGetUniformLocation(program_shadow.get(), "ShadowMap");
        if (loc >= 0){
            glUniform1i(loc, 0);
        } else {
            std::cout << "sha" << std::endl;
            return false;
        }
        glDrawElements(GL_TRIANGLES, mesh.triangles.size() * 3, GL_UNSIGNED_INT,
                        nullptr);
        program_shadow.unbind();
        glBindVertexArray(vao[1]);
        program_point.bind();
        program_point.set_uniform("projectionMatrix", projection);
        program_point.set_uniform("modelviewMatrix", model);
        glDrawArrays(GL_POINTS,0,1);
        program_point.unbind();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_tex[1]);
        glBindVertexArray(vao[2]);
        program_quad.bind();
        program_quad.set_uniform("tex", 1);
        glDrawArrays(GL_TRIANGLES, 0, quad.size() * 3);
        program_quad.unbind();
        window.display();
    }
    return 0;
}
