#include <GL/glew.h>
#include <GL/gl.h>
#include "glutils.hpp"
#include <stdexcept>

namespace glrfw {


void init_gl()
{
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("Could not load glew");
    }
}

std::pair<int,int> gl_version()
{
    std::pair<int,int> version;
    glGetIntegerv(GL_MAJOR_VERSION, &version.first);
    glGetIntegerv(GL_MINOR_VERSION, &version.second);
    return version;
}

std::string gl_version_string()
{
    return std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

std::string glsl_version()
{
    return std::string(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
}

std::string renderer()
{
    return std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
}


std::string vendor()
{
    return std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
}

}
