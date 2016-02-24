#include "shader.hpp"
#include "helper.hpp"

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
    std::string buffer;
    std::ifstream source(file);
    if (source.is_open()){
        buffer = std::string((std::istreambuf_iterator<char>(source)),
                             std::istreambuf_iterator<char>());
    } else {
        THROW_IF(!source.is_open());
    }
    return buffer;
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

shader::shader() :
    handle(nullptr)
{}

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
    THROW_IF(id == 0);
    handle.reset(id);

    std::string source = detail::read_shader(filename);
    const char* data = source.c_str();
    glShaderSource(handle.get(), 1, &data, NULL);
    GLint result = compile_shader(handle.get());
    THROW_IF(result == GL_FALSE);
}

GLuint shader::get() const
{
    return handle.get();
}

program::program(shader vertex) :
    linked(false),
    vertex(std::move(vertex))
{
    std::cout << vertex.get() << std::endl;
    THROW_IF(vertex.get() == 0);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0);
    handle.reset(id);
    glAttachShader(handle.get(),vertex.get());
}


program::program(shader vertex, shader fragment) :
    linked(false),
    vertex(std::move(vertex)),
    fragment(std::move(fragment))
                  
{
    THROW_IF(vertex.get() == 0);
    THROW_IF(fragment.get() == 0);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0);
    handle.reset(id);
    glAttachShader(handle.get(), vertex.get());
    glAttachShader(handle.get(), fragment.get());
}


program::program(shader vertex, shader fragment, shader geometry):
    linked(false),
    vertex(std::move(vertex)),
    fragment(std::move(fragment)),
    geometry(std::move(geometry))
    
{
    THROW_IF(vertex.get() == 0);
    THROW_IF(fragment.get() == 0);
    THROW_IF(geometry.get() == 0);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0);
    handle.reset(id);
    glAttachShader(handle.get(), vertex.get());
    glAttachShader(handle.get(), fragment.get());
    glAttachShader(handle.get(), geometry.get());
}

void program::attach_vertex_shader(shader vertex_shader)
{
    THROW_IF(linked);
    vertex = std::move(vertex_shader);
}

void program::attach_fragment_shader(shader fragment_shader)
{
    THROW_IF(linked);
    fragment = std::move(fragment_shader);

}

void program::attach_geometry_shader(shader geometry_shader)
{
    THROW_IF(linked);
    geometry = std::move(geometry_shader);

}

}
