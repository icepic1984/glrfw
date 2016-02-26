#ifndef HELPER_HPP
#define HELPER_HPP

#pragma GCC system_header

#include <sstream>
#include <SFML/Window.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <iostream>

namespace glrfw {

enum class error_type {
    already_created,
    already_compiled,
    already_linked,
    not_compiled,
    not_linked,
    not_created,
    no_source,
    file_not_found
};

struct gl_error : std::runtime_error {
    gl_error(const std::string& msg, error_type error)
        : std::runtime_error(msg), type(error)
    {
    }

    error_type type;
} ;

#define THROW_IF(EXP, TYPE)                                                    \
    do {                                                                       \
        if (EXP) {                                                             \
            std::ostringstream ss;                                             \
            ss << "Error on line " << __LINE__ << ": " << __PRETTY_FUNCTION__  \
               << " Failed: " << #EXP << std::endl;                            \
            throw gl_error(ss.str(), TYPE);                                    \
        }                                                                      \
    } while (0)
}
#endif
