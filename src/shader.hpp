#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <streambuf>

namespace glrfw {

namespace detail {

struct gl_handle {
    GLuint x;

    gl_handle(std::nullptr_t = nullptr) : x(0) {}

    gl_handle(GLuint x) : x(x) {}

    operator GLuint() const {
        return x;
    }

    friend bool operator == (gl_handle x, gl_handle y) {
        return x.x == x.x;
    }
    friend bool operator != (gl_handle x, gl_handle y){
        return x.x != y.x;
    }
};

template <void (*func)(GLuint)>
struct gl_deleter{
    typedef gl_handle pointer;

    void operator()(gl_handle p) {
        std::cout << "Destory: "<<p.x << std::endl;
        func(p.x);
    }
};

void delete_shader(GLuint p);

void delete_program(GLuint p);

std::string read_shader(const std::string& file);

std::string get_compile_log(GLuint id);

GLint compile_shader(GLuint id);

} // end namespace detail


enum class shader_type { vertex, fragment, geometry };

class shader {
public:
    shader(shader_type type, const std::string& filename);

    shader();

    GLuint get() const;

private:
    typedef std::unique_ptr<detail::gl_handle,
                            detail::gl_deleter<detail::delete_shader>> shader_handle_t;

    std::string filename;
    shader_handle_t handle;
};

class program {
public:
    program(shader vertex);

    program(shader vertex, shader fragment);

    program(shader vertex, shader fragment, shader geometry);

    void attach_vertex_shader(shader vertex);

    void attach_fragment_shader(shader fragment);

    void attach_geometry_shader(shader geometry);
    
private:
    typedef std::unique_ptr<detail::gl_handle,
                            detail::gl_deleter<detail::delete_program>> program_handle_t;
    
    bool linked;
    shader vertex;
    shader fragment;
    shader geometry;
    program_handle_t handle;
};

}


#endif
