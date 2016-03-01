#include <GL/glew.h>
#include <GL/gl.h>
#include "glutils.hpp"
#include <stdexcept>
#include <iostream>

namespace glrfw {

namespace {

std::string source_to_str(GLenum source) 
{
    switch(source) {
    case GL_DEBUG_SOURCE_API:
        return "DEBUG_SOURCE_API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "DEBUG_SOURCE_WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "DEBUG_SOURCE_SHADER_COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "DEBUG_SOURCE_THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
        return "DEBUG_SOURCE_APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
        return "DEBUG_SOURCE_OTHER";
    default:
        return "DEBUG_INVALID_SOURCE_CODE";
    }
}
    

std::string glerror_to_str(GLenum err)
{
    switch (err) {
    case GL_NO_ERROR:
        return std::string("Error: GL_NO_ERROR");
    case GL_INVALID_ENUM:
        return std::string("Error: GL_INVALID_VALUE");
    case GL_INVALID_OPERATION:
        return std::string("Error: GL_INVALID_OPERATION");
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return std::string("Error: GL_INVALID_FRAMEBUFFER_OPERATION");
    case GL_OUT_OF_MEMORY:
        return std::string("Error: GL_OUT_OF_MEMORY");
    case GL_STACK_UNDERFLOW:
        return std::string("Error: GL_STACK_UNDERFLOW");
    case GL_STACK_OVERFLOW:
        return std::string("Error: GL_STACK_OVERFLOW");
    default:
        return std::string("Error: GL_INVALID_ERROR_CODE");
    }
}

} //end of anonymous namespace

void init_gl()
{
    glewExperimental = true;
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
   return std::string(reinterpret_cast<const char*>(
        glGetString(GL_SHADING_LANGUAGE_VERSION)));
}

std::string renderer()
{
    return std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
}


std::string vendor()
{
    return std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
}

std::string get_error()
{
    GLenum err = glGetError();
    return glerror_to_str(err);
}

void GLAPIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar* message, const void* userParam)
{
    std::cout << "Message: " << message << " Id: " << id << std::endl;
}
}
