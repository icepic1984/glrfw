#include "shader.hpp"

namespace glrfw {

namespace detail {

void delete_shader(GLuint p)
{
    glDeleteShader(p);
}

void delete_program(GLuint p)
{
    glDeleteProgram(p);
}


std::string read_shader(const std::string& file)
{
    std::ifstream source(file);
    return std::string((std::istreambuf_iterator<char>(source)),
                       std::istreambuf_iterator<char>());
}

std::string get_compile_log(GLuint id)
{
    GLint log_len;
    std::string log;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) {
        log = std::string(log_len, ' ');
        GLsizei written;
        glGetShaderInfoLog(id, log_len, &written, &log[0]);
    }
    return log;
}
    
GLint compile_shader(GLuint id)
{
    glCompileShader(id);
    GLint result;
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    return result;
}

} //end namespace detail

shader::shader(shader_type type, const std::string& filename) :
    filename(filename)
{
    GLuint id = 0;
    switch (type){
    case shader_type::vertex:
        id = glCreateShader(GL_VERTEX_SHADER);
        break;
    case shader_type::fragment:
        id = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    case shader_type::geometry:
        id = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    }
    if (id == 0)
        throw std::runtime_error("Could not create shader.");
    handle.reset(id);

    std::string source = detail::read_shader(filename);
    const char* data = source.c_str();
    glShaderSource(handle.get(), 1, &data, NULL);
    GLint result = compile_shader(handle.get());
    if (result == GL_FALSE) {
        std::cerr << detail::get_compile_log(handle.get()) << std::endl;
        throw std::runtime_error("Could not compile shader.");
    } 
}

GLuint shader::get()
{
    return handle.get();
}

program::program(shader vertex)
{}


program::program(shader vertex, shader fragment)
{}


program::program(shader vertex, shader fragment, shader geometry)
{}

}
