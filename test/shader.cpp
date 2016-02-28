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
    std::cout << static_cast<int>(ex.type) << std::endl;
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

bool shader_is_already_created(const glrfw::gl_error& ex)
{
    return ex.type == glrfw::error_type::shader_already_created;
}

BOOST_AUTO_TEST_CASE(gl_handle)
{
	glrfw::detail::gl_handle handle1;
	BOOST_CHECK(handle1.x_ == 0);
	glrfw::detail::gl_handle handle2(10);
	BOOST_CHECK(handle1 != handle2);
	handle1 = glrfw::detail::gl_handle(10);
    BOOST_CHECK(handle1 == handle2);
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
    glrfw::shader shader_test;
    BOOST_CHECK(!shader_test.is_compiled());
    BOOST_CHECK(shader_test.get() == 0);
    BOOST_CHECK_EXCEPTION(shader_test.compile(), glrfw::gl_error,
                          shader_not_created);
    shader_test.create();
    BOOST_CHECK_EXCEPTION(shader_test.compile(), glrfw::gl_error,
                          no_shader_source);
    BOOST_CHECK_EXCEPTION(shader_test.create(), glrfw::gl_error,
                          shader_is_already_created);
    // test shader with errors
    shader_test.set_source("void main");
    BOOST_CHECK_EXCEPTION(shader_test.compile(), glrfw::gl_error,
                          shader_not_compiled);
    shader_test.set_source("void main() { }");
    shader_test.compile();
    BOOST_CHECK_EXCEPTION(shader_test.set_type(glrfw::shader_type::vertex),
                          glrfw::gl_error, shader_is_compiled);
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
    BOOST_CHECK(shader_test.is_created());
    BOOST_CHECK(shader_test.is_compiled());
}


BOOST_AUTO_TEST_CASE(program_construction)
{
	glrfw::program prog;
	BOOST_CHECK(prog.get() == 0);
	BOOST_CHECK(!prog.is_linked());
	
	

}


