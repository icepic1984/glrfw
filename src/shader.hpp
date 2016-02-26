#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <streambuf>

namespace glrfw {


enum class shader_type { vertex, fragment, geometry };

namespace detail {

struct gl_handle {
    GLuint x_;

    gl_handle(std::nullptr_t = nullptr) : x_(0) {}

    gl_handle(GLuint x) : x_(x) {}

    operator GLuint() const {
        return x_;
    }

    friend bool operator == (gl_handle x, gl_handle y) {
        return x.x_ == y.x_;
    }
    friend bool operator != (gl_handle x, gl_handle y){
        return x.x_ != y.x_;
    }
};

template <void (*func)(GLuint)>
struct gl_deleter{
    typedef gl_handle pointer;

    void operator()(gl_handle p) {
        std::cout << "Destory: "<<p.x_ << std::endl;
        func(p.x_);
    }
};

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
    shader(shader_type type, const std::string& filename);

    shader();

    shader_type type() const;

    void compile();

    bool is_compiled() const;

    void set_type(shader_type type);

    std::string source();
    
    void set_source(const std::string& source);

    GLuint get() const;

private:
    typedef std::unique_ptr<detail::gl_handle,
                            detail::gl_deleter<detail::delete_shader>> shader_handle_t;

    bool compiled_;
    std::string source_;
    shader_type type_;
    std::string filename_;
    shader_handle_t handle_;
};

class program {
public:
    program(shader vertex);

    program(shader vertex, shader fragment);

    program(shader vertex, shader fragment, shader geometry);

    void attach_vertex_shader(shader vertex);

    void attach_fragment_shader(shader fragment);

    void attach_geometry_shader(shader geometry);

    void set_attribute(GLuint index, const std::string& name);

    void set_frag_data_location(GLuint index, const std::string& name);

    void link();

    bool is_linked() const;
    
private:
    typedef std::unique_ptr<detail::gl_handle,
                            detail::gl_deleter<detail::delete_program>> program_handle_t;
    
    bool linked_;
    shader vertex_;
    shader fragment_;
    shader geometry_;
    program_handle_t handle_;
};

}


#endif
