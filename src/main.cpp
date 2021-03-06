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

glm::vec3 light_pos(88.30f, 176.2f, 147.85f);

glm::vec3 view_pos(0, 176.1f, 147.85f);

std::vector<glm::vec3> quad 
{
    {-1.0f,-1.0f,0.0f},
    {-0.5f,-1.0f,0.0f},
    {-1.0f,-0.5f,0.0f},
    {-1.0f,-0.5f,0.0f},
    {-0.5f,-1.0f,0.0f},
    {-0.5f,-0.5f,0.0f}
};

std::vector<glm::vec3> ground 
{
    {-1.0f,0.0f,1.0f},
    {-1.0f,0.0f,-1.0f},
    {1.0f,0.0f,-1.0f},
    {1.0f,0.0f,-1.0f},
    {1.0f,0.0f,1.0f},
    {-1.0f,0.0f,1.0f}
};
    
std::vector<glm::vec3> sensor
{
    {88.30f, 176.2f, 147.85f},
    {0.0f, 176.2f, 147.85f},
    {0.0f, 176.2f, 147.85f},
    {0.0f,0.0f,0.0f},
    {0.0f,0.0f,0.0f},
    {88.30f, 176.2f, 147.85f},
    {88.30f, 176.2f, 147.85f},
    {88.30f, 176.2f, -20.0f}
        
};


glm::vec3 getArcBall(const glm::vec2& pos, int width, int height)
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

void update(const glm::mat4& view, glm::mat4& model, glm::mat3& normal,
            const glm::vec2& start_pos, const glm::vec2& cur_pos, int width,
            int height)
{
    glm::vec3 va = getArcBall(start_pos, width, height);
    glm::vec3 vb = getArcBall(cur_pos, width, height);
    float angle =
        static_cast<float>(std::acos(std::min(1.0f, glm::dot(va, vb))));
    glm::vec3 axis = glm::cross(va, vb);
    glm::mat3 eye = glm::inverse(glm::mat3(view * model));
    glm::vec3 obj_axis = eye * axis;
    model = glm::rotate(model, 0.1f * glm::degrees(angle), obj_axis);
}


int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Setup windows and create context
    glm::ivec2 viewport_size(800,600);
    glm::ivec2 depthmap_size(2048,2048);
    int delta = 50;
    
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

    glrfw::mesh ground_mesh;
    ground_mesh.add_triangle(glm::vec3(-100.0f,100.0f,-20.0f),
                             glm::vec3(-100.0f,-100.0f,-20.0f),
                             glm::vec3(100.0f,-100.0f,-20.0f));

    ground_mesh.add_triangle(glm::vec3(100.0f,-100.0f,-20.0f),
                             glm::vec3(100.0f,100.0f,-20.0f),
                             glm::vec3(-100.0f,100.0f,-20.0f));
    ground_mesh.calculate_normals();

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

    glrfw::shader vertex_ground(glrfw::shader_type::vertex,
                               glrfw::resource_path + std::string("ground.vert"));
    glrfw::shader fragment_ground(glrfw::shader_type::fragment,
                                  glrfw::resource_path +
                                      std::string("ground.frag"));
    glrfw::program program_ground(std::move(vertex_ground),
                                  std::move(fragment_ground));


    glrfw::shader vertex_quad(glrfw::shader_type::vertex,
                               glrfw::resource_path + std::string("quad.vert"));
    glrfw::shader fragment_quad(glrfw::shader_type::fragment,
                                 glrfw::resource_path +
                                     std::string("quad.frag"));
    glrfw::program program_quad(std::move(vertex_quad),
                                 std::move(fragment_quad));

    glrfw::shader vertex_lines(glrfw::shader_type::vertex,
                               glrfw::resource_path + std::string("lines.vert"));
    glrfw::shader fragment_lines(glrfw::shader_type::fragment,
                                 glrfw::resource_path +
                                     std::string("lines.frag"));
    glrfw::program program_lines(std::move(vertex_lines),
                                 std::move(fragment_lines));

    // create matrices
    auto projection =
        glm::perspective(45.0f, static_cast<float>(viewport_size.x) /
                                    static_cast<float>(viewport_size.y),
                         0.1f, 1000.0f);
    auto view =
        glm::lookAt(view_pos, glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f));
    auto model = glm::mat4(1.0f);
    
    auto normal = glm::transpose(glm::inverse(glm::mat3(view*model)));


    std::cout << "Program: " << std::endl;
    program.set_attribute(0, "in_Position");
    program.set_attribute(1, "in_Normals");
    program.link();
    program.bind();
    program.set_uniform("projectionMatrix", projection);
    program.set_uniform("modelviewMatrix", view * model);
    program.set_uniform("normalMatrix", normal);
    std::cout << program.attributes() << std::endl;
    std::cout << program.uniforms() << std::endl;
    program.unbind();

    std::cout << "Depth Program: " << std::endl;
    program_depth.set_attribute(0, "in_Position");
    program_depth.link();
    program_depth.bind();
    program_depth.set_uniform("projectionMatrix", projection);
    program_depth.set_uniform("modelviewMatrix", view * model);
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

    std::cout << "Ground Program: " << std::endl;
    program_ground.set_attribute(0, "in_Position");
    program_ground.set_attribute(1, "in_Normals");
    program_ground.link();
    program_ground.bind();
    std::cout << program_ground.attributes() << std::endl;
    std::cout << program_ground.uniforms() << std::endl;
    program_ground.unbind();

    std::cout << "Point Program: " << std::endl;
    program_point.set_attribute(0, "in_Position");
    program_point.link();
    program_point.bind();
    program_point.set_uniform("projectionMatrix", projection);
    program_point.set_uniform("modelviewMatrix", model);
    std::cout << program_point.attributes() << std::endl;
    std::cout << program_point.uniforms() << std::endl;
    program_point.unbind();

    std::cout << "Lines Program: " << std::endl;
    program_lines.set_attribute(0, "in_Position");
    program_lines.link();
    program_lines.bind();
    std::cout << program_point.attributes() << std::endl;
    std::cout << program_point.uniforms() << std::endl;
    program_lines.unbind();

    
    // Set up view port
    glViewport(0,0,viewport_size.x,viewport_size.y);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
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
    GLuint vbos[9];
    glGenBuffers(9,&vbos[0]);
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

    glBindBuffer(GL_ARRAY_BUFFER,vbos[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * sensor.size(), &sensor[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[6]);
    glBufferData(GL_ARRAY_BUFFER, ground_mesh.vertices.size() * sizeof(glm::vec3),
                 &ground_mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[7]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 ground_mesh.triangles.size() * sizeof(glm::ivec3),
                 &ground_mesh.triangles[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[8]);
    glBufferData(GL_ARRAY_BUFFER,
                 ground_mesh.vertex_normals.size() * sizeof(glm::vec3),
                 &ground_mesh.vertex_normals[0], GL_STATIC_DRAW);

    // Generate vertex array objects and bind mesh vbos to the current
    // vao
    GLuint vao[5];
    glGenVertexArrays(5, &vao[0]);

    // Jaw 
    glBindVertexArray(vao[0]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[2]);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbos[1]);
    
    // Point for light source
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0); 

    // Quad for depth map
    glBindVertexArray(vao[2]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[4]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Sensor field
    glBindVertexArray(vao[3]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[5]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Ground plane
    glBindVertexArray(vao[4]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[6]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
    glBindBuffer(GL_ARRAY_BUFFER,vbos[8]);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[7]);

    bool running = true;
    bool mouse_pressed = false;

    glm::vec2 start_pos;
    glm::vec2 cur_pos;

    float rotation_angle = 0.0f;
    bool move_light = false;
    
    bool red_shadow = false;

    glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5,
                         0.0, 0.5, 0.5, 0.5, 1.0);

    auto depth_projection =
        glm::perspective(45.0f, static_cast<float>(depthmap_size.x) /
                                    static_cast<float>(depthmap_size.y),
                         0.1f, 1000.0f);
    auto depth_view =
        glm::lookAt(light_pos, glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
    
    auto depth_normal = glm::transpose(glm::inverse(glm::mat3(depth_view*model)));

    auto shadow_matrix = biasMatrix * depth_projection * depth_view * model;

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
                view_pos += view_pos * event.mouseWheel.delta * 0.05f;
                view = glm::lookAt(view_pos, glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, -1.0f, 0.0f));

                normal = glm::transpose(glm::inverse(glm::mat3(view * model)));
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
                    program_shadow.bind();
                    program_shadow.set_uniform("red_shadow",red_shadow ? 1 : 0);
                    program_shadow.unbind();
                    red_shadow = !red_shadow;

                }
            }
        }

        if (mouse_pressed) {
            if( cur_pos != start_pos) {
                update(view, model, normal, start_pos, cur_pos, viewport_size.x,
                       viewport_size.y);
                normal = glm::transpose(glm::inverse(glm::mat3(view * model)));
                shadow_matrix = biasMatrix * depth_projection * depth_view * model;
                start_pos = cur_pos;
            }
        }


        if ( move_light) {
            rotation_angle += 1;
            if (rotation_angle > 360)
                rotation_angle = 0.0;
            light_pos.x = 100.0f * static_cast<float>(
                                       std::cos(glm::radians(rotation_angle)));
            light_pos.z = 100.0f * static_cast<float>(
                                       std::sin(glm::radians(rotation_angle)));
            depth_view =
                glm::lookAt(light_pos, glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
            depth_normal = glm::transpose(glm::inverse(glm::mat3(depth_view*model)));
            shadow_matrix = biasMatrix * depth_projection * depth_view * model;

            glBindVertexArray(vao[1]);
            glBindBuffer(GL_ARRAY_BUFFER,vbos[3]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3),&light_pos);

        }
        normal = glm::transpose(glm::inverse(glm::mat3(view * model)));
        shadow_matrix = biasMatrix * depth_projection * depth_view * model;


        // Configure framebuffer for depth map
        glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
        glViewport(0,0,depthmap_size.x,depthmap_size.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonOffset(5.5f,100.0f);
        glEnable(GL_POLYGON_OFFSET_FILL);
        
        // Render jaw to depth map from light source
        glBindVertexArray(vao[0]);
        program_depth.bind();
        program_depth.set_uniform("projectionMatrix", depth_projection);
        program_depth.set_uniform("modelviewMatrix", depth_view * model);
        glDrawElements(GL_TRIANGLES, mesh.triangles.size() * 3,
                        GL_UNSIGNED_INT, nullptr);

        // Render ground plane from light source
        glBindVertexArray(vao[4]);
        program_depth.set_uniform("projectionMatrix", depth_projection);
        program_depth.set_uniform("modelviewMatrix", depth_view);
        glDrawElements(GL_TRIANGLES, ground_mesh.triangles.size() * 3,
                       GL_UNSIGNED_INT, nullptr);
        program_depth.unbind();
        glDisable(GL_POLYGON_OFFSET_FILL);
        
        // Configure framebuffer for helper window
        glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
        glViewport(0,0,depthmap_size.x,depthmap_size.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw jaw from light source
        glBindVertexArray(vao[0]);
        program_depth.bind();
        program_depth.set_uniform("projectionMatrix", depth_projection);
        program_depth.set_uniform("modelviewMatrix", depth_view * model);
        glDrawElements(GL_TRIANGLES, mesh.triangles.size() * 3,
                        GL_UNSIGNED_INT, nullptr);

        // Draw ground plane from light source
        glBindVertexArray(vao[4]);
        program_depth.set_uniform("projectionMatrix", depth_projection);
        program_depth.set_uniform("modelviewMatrix", depth_view );
        glDrawElements(GL_TRIANGLES, ground_mesh.triangles.size() * 3,
                       GL_UNSIGNED_INT, nullptr);
        program_depth.unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport_size.x, viewport_size.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render  jaw with shadows
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth_tex[0]);
        glBindVertexArray(vao[0]);
        program_shadow.bind();
        program_shadow.set_uniform("projectionMatrix", projection);
        program_shadow.set_uniform("modelviewMatrix", view * model);
        program_shadow.set_uniform("normalMatrix", normal);
        program_shadow.set_uniform("lightpos",light_pos);
        program_shadow.set_uniform("shadowMatrix",shadow_matrix);
        program_shadow.set_uniform("ShadowMap", 0);
        glDrawElements(GL_TRIANGLES, mesh.triangles.size() * 3, GL_UNSIGNED_INT,
                        nullptr);

        // Render ground plane with shadows
        glBindVertexArray(vao[4]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth_tex[0]);
        program_ground.bind();
        program_ground.set_uniform("projectionMatrix", projection);
        program_ground.set_uniform("modelviewMatrix", view );
        program_ground.set_uniform(
            "normalMatrix", glm::transpose(glm::inverse(glm::mat3(view))));
        
        program_ground.set_uniform("lightpos",light_pos);
        program_ground.set_uniform("shadowMatrix",
                                   biasMatrix * depth_projection * depth_view);
        program_ground.set_uniform("ShadowMap", 0);
        glDrawElements(GL_TRIANGLES, ground_mesh.triangles.size() * 3,
                       GL_UNSIGNED_INT, nullptr);
        program_shadow.unbind();

        // Render point for light source
        glBindVertexArray(vao[1]);
        program_point.bind();
        program_point.set_uniform("projectionMatrix", projection);
        program_point.set_uniform("modelviewMatrix",  view  );
        glDrawArrays(GL_POINTS,0,1);
        program_point.unbind();

        // Render lines for sensor field
        glBindVertexArray(vao[3]);
        program_lines.bind();
        program_lines.set_uniform("projectionMatrix", projection);
        program_lines.set_uniform("modelviewMatrix", view );
        glDrawArrays(GL_LINES,0, sensor.size());
        program_lines.unbind();

        // Render depth map to bottom left of screen
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_tex[1]);
        glBindVertexArray(vao[2]);
        program_quad.bind();
        program_quad.set_uniform("tex", 1);
        glDrawArrays(GL_TRIANGLES, 0, quad.size());
        program_quad.unbind();
        window.display();
    }
    return 0;
}
