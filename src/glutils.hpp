#ifndef GLUTILS_HPP
#define GLUTILS_HPP

#include <string>
#include <utility>

namespace glrfw {

void init_gl();

std::pair<int, int> gl_version();

std::string gl_version_string();

std::string glsl_version();

std::string renderer();

std::string vendor();

std::string get_error();

void GLAPIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar* message, const void* userParam);
}

#endif
