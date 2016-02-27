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
        THROW_IF(!source.is_open(), error_type::file_not_found);
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

std::string get_link_log(GLuint id)
{
    GLint log_len;
    std::string log;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) {
        log = std::string(log_len,' ');
        GLsizei written;
        glGetProgramInfoLog(id, log_len, &written, &log[0]);
    }
    return log;
}

GLint create_shader(shader_type type)
{
    GLint id = 0;
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
    default:
        id = 0;
    }
    return id;
}

GLint link_program(GLuint id)
{
    glLinkProgram(id);
    GLint result;
    glGetShaderiv(id, GL_LINK_STATUS, &result);
    return result;
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
    compiled_(false),
    source_(std::string()),
    type_(shader_type::vertex),
    filename_(std::string()),
    handle_(nullptr)
{}

shader::shader(shader_type type, const std::string& filename) :
    compiled_(false),
    source_(std::string()),
    type_(type),
    filename_(filename),
    handle_(nullptr)
{
    GLuint id = detail::create_shader(type_);
    THROW_IF(id == 0, error_type::not_created);
    handle_.reset(id);
    source_ = detail::read_shader(filename_);
    const char* data = source_.c_str();
    glShaderSource(handle_.get(), 1, &data, NULL);
    GLint result = compile_shader(handle_.get());
    if (result == GL_FALSE)
        std::cerr << get_compile_log(handle_.get()) <<std::endl;
    THROW_IF(result == GL_FALSE, error_type::not_compiled);
    compiled_ = true;
}

shader_type shader::type() const
{
    return type_;
}

void shader::set_type(shader_type type)
{
    THROW_IF(compiled_, error_type::already_compiled);
    type_ = type;
}

GLuint shader::get() const
{
    return handle_.get();
}

std::string shader::source()
{
    return source_;
}

bool shader::is_created()
{
    return get() != 0;
}

bool shader::is_compiled() const
{
    return compiled_;
}

void shader::set_source(const std::string& source)
{
    source_ = source;
}

void shader::create()
{
    THROW_IF(get() != 0, error_type::already_created);
    GLuint id = detail::create_shader(type_);
    THROW_IF(id == 0,error_type::not_created);
    handle_.reset(id);
}
	
void shader::compile()
{
	THROW_IF(get() == 0, error_type::not_created);
    THROW_IF(compiled_, error_type::already_compiled);
    THROW_IF(source_.empty(), error_type::no_source);
    const char* data = source_.c_str();
    glShaderSource(handle_.get(), 1, &data, NULL);
    GLint result = compile_shader(handle_.get());
    if (result == GL_FALSE)
        std::cerr << get_compile_log(handle_.get()) <<std::endl;
    THROW_IF(result == GL_FALSE,error_type::not_compiled);
    compiled_ = true;
}

program::program(shader vertex) :
    linked_(false),
    vertex_(std::move(vertex)),
    fragment_(shader()),
    geometry_(shader()),
    handle_(nullptr)
    
{
	THROW_IF(vertex_.get() == 0,error_type::not_created);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0,error_type::not_created);
    handle_.reset(id);
    glAttachShader(handle_.get(), vertex_.get());
}


program::program(shader vertex, shader fragment) :
    linked_(false),
    vertex_(std::move(vertex)),
    fragment_(std::move(fragment)),
    geometry_(shader()),
    handle_(nullptr)
                  
{
    THROW_IF(vertex_.get() == 0, error_type::not_created);
    THROW_IF(fragment_.get() == 0, error_type::not_created);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0, error_type::not_created);
    handle_.reset(id);
    glAttachShader(handle_.get(), vertex_.get());
    glAttachShader(handle_.get(), fragment_.get());
}


program::program(shader vertex, shader fragment, shader geometry):
    linked_(false),
    vertex_(std::move(vertex)),
    fragment_(std::move(fragment)),
    geometry_(std::move(geometry)),
    handle_(nullptr)
    
{
    THROW_IF(vertex_.get() == 0, error_type::not_created);
    THROW_IF(fragment_.get() == 0, error_type::not_created);
    THROW_IF(geometry_.get() == 0, error_type::not_created);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0, error_type::not_created);
    handle_.reset(id);
    glAttachShader(handle_.get(), vertex.get());
    glAttachShader(handle_.get(), fragment.get());
    glAttachShader(handle_.get(), geometry.get());
}

void program::attach_vertex_shader(shader vertex_shader)
{
    THROW_IF(linked_, error_type::already_linked);
    vertex_ = std::move(vertex_shader);
}

void program::attach_fragment_shader(shader fragment_shader)
{
    THROW_IF(linked_, error_type::already_linked);
    fragment_ = std::move(fragment_shader);
}

void program::attach_geometry_shader(shader geometry_shader)
{
    THROW_IF(linked_, error_type::already_linked);
    geometry_ = std::move(geometry_shader);
}

bool program::is_linked() const
{
    return linked_;
}

void program::link()
{
    THROW_IF((vertex_.get() == 0) && (fragment_.get() == 0) &&
             (geometry_.get() == 0),error_type::not_created);
    GLint result = link_program(handle_.get());
    if (result == GL_FALSE)
        std::cerr << get_link_log(handle_.get()) <<std::endl;
    THROW_IF(result == GL_FALSE, error_type::not_linked);
    linked_ = true;
}

void program::set_attribute(GLuint index, const std::string& name)
{
    glBindAttribLocation(handle_.get(), index, name.c_str());
}

void program::set_frag_data_location(GLuint index, const std::string& name)
{
    glBindFragDataLocation(handle_.get(), index, name.c_str());
}



}
