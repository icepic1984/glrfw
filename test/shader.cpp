#define BOOST_TEST_MODULE shader

#include <boost/test/unit_test.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <glutils.hpp>
#include <helper.hpp>
#include <shader.hpp>

bool no_file(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::file_not_found;
}

bool no_source_found(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::no_source;
}

bool is_not_compiled(const glrfw::gl_error& ex)
{
    std::cout << static_cast<int>(ex.type) << std::endl;
    return ex.type == glrfw::error_type::not_compiled;
}

bool is_compiled(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::already_compiled;
}

bool not_created(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::not_created;
}

bool is_already_created(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::already_created;
}

BOOST_AUTO_TEST_CASE(default_construction)
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
    glrfw::shader shader_test;
    BOOST_CHECK(!shader_test.is_compiled());
    BOOST_CHECK(shader_test.get() == 0);
    BOOST_CHECK_EXCEPTION(shader_test.set_type(glrfw::shader_type::vertex),
                          glrfw::gl_error, not_created);
    BOOST_CHECK_EXCEPTION(shader_test.compile(), glrfw::gl_error, not_created);
    shader_test.create();
    BOOST_CHECK_EXCEPTION(shader_test.compile(), glrfw::gl_error,
                          no_source_found);
    BOOST_CHECK_EXCEPTION(shader_test.create(), glrfw::gl_error,
                          is_already_created);
    // test shader with errors
    shader_test.set_source("void main");
    BOOST_CHECK_EXCEPTION(shader_test.compile(), glrfw::gl_error,
                          is_not_compiled);
    shader_test.set_source("void main() { }");
    shader_test.compile();
    BOOST_CHECK_EXCEPTION(shader_test.set_type(glrfw::shader_type::vertex),
                          glrfw::gl_error, is_compiled);
    BOOST_CHECK_EXCEPTION(shader_test.compile();, glrfw::gl_error, is_compiled);
}

BOOST_AUTO_TEST_CASE(construction)
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
                          glrfw::gl_error, is_not_compiled);

    glrfw::shader shader_test(glrfw::shader_type::vertex,
                              "../test/resources/test_ok.vert");
    BOOST_CHECK(shader_test.is_created());
    BOOST_CHECK(shader_test.is_compiled());

}
