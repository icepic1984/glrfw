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

#define THROW_IF(EXP)                                                          \
    do {                                                                       \
        if (EXP) {                                                             \
            std::ostringstream ss;                                             \
            ss << "Error on: " << __LINE__ << ": " << __PRETTY_FUNCTION__      \
               << " Failed: " << #EXP << std::endl;                            \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
    } while (0)

#endif
