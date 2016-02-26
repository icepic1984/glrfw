#define BOOST_TEST_MODULE parser

#include <boost/test/unit_test.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <glutils.hpp>
#include <helper.hpp>
#include <shader.hpp>

BOOST_AUTO_TEST_CASE(shader)
{
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
    glrfw::shader shader_test;
    BOOST_CHECK(!shader_test.is_compiled());
    BOOST_CHECK(shader_test.get() == 0);
}
