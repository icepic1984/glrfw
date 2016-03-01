#define BOOST_TEST_MODULE shader

#include <boost/test/unit_test.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <glutils.hpp>
#include <error.hpp>
#include <shader.hpp>

bool no_file(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::file_not_found;
}

bool no_shader_source(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::no_shader_source;
}

bool shader_not_compiled(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::shader_not_compiled;
}

bool shader_is_compiled(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::shader_already_compiled;
}

bool shader_not_created(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::shader_not_created;
}

bool program_already_linked(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::program_already_linked;
}

BOOST_AUTO_TEST_CASE(shader_default_construction)
{
    // init window
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default,
                      settings);
    window.setVerticalSyncEnabled(true);
    glrfw::init_gl();

    // test default constructor
    glrfw::shader shader_test(glrfw::shader_type::vertex);
    BOOST_CHECK(!shader_test.is_compiled());
    BOOST_CHECK_EXCEPTION(shader_test.compile(), glrfw::gl_error,
                          no_shader_source);
    // test shader with errors
    shader_test.set_source("void main");
    BOOST_CHECK_EXCEPTION(shader_test.compile(), glrfw::gl_error,
                          shader_not_compiled);
    shader_test.set_source("void main() { }");
    shader_test.compile();
    BOOST_CHECK_EXCEPTION(shader_test.compile();
                          , glrfw::gl_error, shader_is_compiled);
}

BOOST_AUTO_TEST_CASE(shader_construction)
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default,
                      settings);
    window.setVerticalSyncEnabled(true);
    glrfw::init_gl();

    BOOST_CHECK_EXCEPTION(glrfw::shader shader_test(glrfw::shader_type::vertex,
                                                    "resources/test_ok"),
                          glrfw::gl_error, no_file);
    BOOST_CHECK_EXCEPTION(glrfw::shader shader_test(glrfw::shader_type::vertex,
                                                    "../test/resources/test_fail.vert"),
                          glrfw::gl_error, shader_not_compiled);

    glrfw::shader shader_test(glrfw::shader_type::vertex,
                              "../test/resources/test_ok.vert");
    BOOST_CHECK(shader_test.is_compiled());
}


BOOST_AUTO_TEST_CASE(program_construction)
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default,
                      settings);
    window.setVerticalSyncEnabled(true);
    glrfw::init_gl();
    glrfw::program prog;
    BOOST_CHECK(prog.get() != 0);
    BOOST_CHECK(!prog.is_linked());
    glrfw::shader vertex_shader_1(glrfw::shader_type::vertex,
                                  "../test/resources/test_ok.vert");
    glrfw::shader vertex_shader_2(glrfw::shader_type::fragment);
    BOOST_CHECK(vertex_shader_1.is_compiled());
    BOOST_CHECK(!vertex_shader_2.is_compiled());
    prog.attach_vertex_shader(std::move(vertex_shader_1));
    BOOST_CHECK_EXCEPTION(prog.attach_vertex_shader(std::move(vertex_shader_2)),
                          glrfw::gl_error, shader_not_compiled);
    vertex_shader_2.set_source("void main() {}");
    vertex_shader_2.compile();
    BOOST_CHECK(vertex_shader_2.is_compiled());
    prog.link();
    BOOST_CHECK(prog.is_linked());
    BOOST_CHECK_EXCEPTION(prog.attach_vertex_shader(std::move(vertex_shader_2)),
                          glrfw::gl_error, program_already_linked);
}

BOOST_AUTO_TEST_CASE(program_attrib)
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default,
                      settings);
    window.setVerticalSyncEnabled(true);
    glrfw::init_gl();

    glrfw::shader vertex(glrfw::shader_type::vertex, "../resources/basic.vert");
    glrfw::shader fragment(glrfw::shader_type::fragment,
                           "../resources/basic.frag");
    glrfw::program prog(std::move(vertex),std::move(fragment));
    prog.set_attribute(0,"VertexPosition");
    prog.set_attribute(1,"VertexColor");
    prog.link();
    BOOST_CHECK(prog.is_linked());
    std::cout << prog.attributes() << std::endl;
    prog.bind();
    prog.set_uniform("model_view",glm::mat4());
    prog.set_uniform("projection",glm::mat4());
    std::cout << prog.uniforms() << std::endl;
}
