#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <unordered_map>
#include "handle.hpp"
#include "error.hpp"

namespace glrfw {

enum class shader_type { vertex, fragment, geometry };

namespace detail {

void delete_shader(GLuint p);

void delete_program(GLuint p);

std::string read_shader(const std::string& file);

std::string get_compile_log(GLuint id);

GLint link_program(GLuint id);

GLint compile_shader(GLuint id);

GLint create_shader(shader_type type);

} // end namespace detail


class shader {
public:
    shader(shader_type type);

    shader(shader_type type, const std::string& filename);

    shader_type type() const;

    void compile();

    void create();

    bool is_compiled() const;

    std::string source();

    void set_source(const std::string& source);

    GLuint get() const;

private:
    typedef std::unique_ptr<detail::gl_handle,
                            detail::gl_deleter<detail::delete_shader>>
        shader_handle_t;

    bool compiled_;

    std::string source_;

    shader_type type_;

    std::string filename_;

    shader_handle_t handle_;
};

class program {

public:
    program();

    program(shader&& vertex);

    program(shader&& vertex, shader&& fragment);

    program(shader&& vertex, shader&& fragment, shader&& geometry);

    void attach_vertex_shader(shader&& vertex);

    void attach_fragment_shader(shader&& fragment);

    void attach_geometry_shader(shader&& geometry);

    void set_attribute(GLuint index, const std::string& name);

    void set_frag_data_location(GLuint index, const std::string& name);

    void bind();

    void unbind();

    void link();

    bool is_linked() const;

    GLuint get() const;

    std::string attributes();

    std::string uniforms();

    void set_uniform(const std::string& name, const glm::mat4& matrix);

    void set_uniform(const std::string& name, const glm::mat3& matrix);

    void set_uniform(const std::string& name, const glm::vec3& vec);

    void set_uniform(const std::string& name, const glm::vec4& vec);

    void set_uniform(const std::string& name, int value);

    void set_uniform(const std::string& name, float value);

    void set_uniform(const std::string& name, bool value);

private:
    typedef std::unique_ptr<detail::gl_handle,
                            detail::gl_deleter<detail::delete_program>>
        program_handle_t;

    struct shader_index {

        enum shader_index_e { vertex, fragment, geometry };
    };

    void insert(shader sh, int index);

    GLuint uniform_location(const std::string& name);

    bool linked_;

    std::unordered_map<int, shader> shaders_;

    std::unordered_map<std::string, GLint> uniform_loc_;

    program_handle_t handle_;
};

} // end namespace glrfw

#endif
