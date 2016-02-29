#include "shader.hpp"
#include <sstream>

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

shader::shader(shader_type type) :
    compiled_(false),
    source_(std::string()),
    type_(type),
    filename_(std::string()),
    handle_(nullptr)
{
    GLuint id = detail::create_shader(type_);
    THROW_IF(id == 0,error_type::shader_not_created);
    handle_.reset(id);
}

shader::shader(shader_type type, const std::string& filename) :
    compiled_(false),
    source_(std::string()),
    type_(type),
    filename_(filename),
    handle_(nullptr)
{
    GLuint id = detail::create_shader(type_);
    THROW_IF(id == 0, error_type::shader_not_created);
    handle_.reset(id);
    source_ = detail::read_shader(filename_);
    const char* data = source_.c_str();
    glShaderSource(handle_.get(), 1, &data, NULL);
    GLint result = compile_shader(handle_.get());
    if (result == GL_FALSE)
        std::cerr << get_compile_log(handle_.get()) <<std::endl;
    THROW_IF(result == GL_FALSE, error_type::shader_not_compiled);
    compiled_ = true;
}

shader_type shader::type() const
{
    return type_;
}

GLuint shader::get() const
{
    return handle_.get();
}

std::string shader::source()
{
    return source_;
}

bool shader::is_compiled() const
{
    return compiled_;
}

void shader::set_source(const std::string& source)
{
    source_ = source;
}

void shader::compile()
{
    THROW_IF(compiled_, error_type::shader_already_compiled);
    THROW_IF(source_.empty(), error_type::no_shader_source);
    const char* data = source_.c_str();
    glShaderSource(handle_.get(), 1, &data, NULL);
    GLint result = compile_shader(handle_.get());
    if (result == GL_FALSE)
        std::cerr << get_compile_log(handle_.get()) <<std::endl;
    THROW_IF(result == GL_FALSE,error_type::shader_not_compiled);
    compiled_ = true;
}

program::program():
    linked_(false),
    shaders_(std::unordered_map<int,shader>()),
    uniform_loc_(std::unordered_map<std::string, GLint>()),
    handle_(nullptr)
{
    GLuint id = glCreateProgram();
    THROW_IF(id == 0, error_type::program_not_created);
    handle_.reset(id);
}

program::program(shader&& vertex) :
    linked_(false),
    shaders_(std::unordered_map<int,shader>()),
    uniform_loc_(std::unordered_map<std::string, GLint>()),
    handle_(nullptr)
{
    THROW_IF(!vertex.is_compiled(),error_type::shader_not_compiled);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0,error_type::program_not_created);
    handle_.reset(id);
    shaders_.insert(std::make_pair(shader_index::vertex,std::move(vertex)));
    glAttachShader(handle_.get(), shaders_.at(shader_index::vertex).get());
}


program::program(shader&& vertex, shader&& fragment) :
    linked_(false),
    shaders_(std::unordered_map<int,shader>()),
    uniform_loc_(std::unordered_map<std::string, GLint>()),
    handle_(nullptr)
{
    THROW_IF(!vertex.is_compiled(), error_type::shader_not_compiled);
    THROW_IF(!fragment.is_compiled(), error_type::shader_not_compiled);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0, error_type::program_not_created);
    handle_.reset(id);
    shaders_.insert(std::make_pair(shader_index::vertex,std::move(vertex)));
    shaders_.insert(std::make_pair(shader_index::fragment,std::move(fragment)));
    glAttachShader(handle_.get(), shaders_.at(shader_index::vertex).get());
    glAttachShader(handle_.get(), shaders_.at(shader_index::fragment).get());
}


program::program(shader&& vertex, shader&& fragment, shader&& geometry):
    linked_(false),
    shaders_(std::unordered_map<int,shader>()),
    uniform_loc_(std::unordered_map<std::string, GLint>()),
    handle_(nullptr)
{
    THROW_IF(!vertex.is_compiled(), error_type::shader_not_compiled);
    THROW_IF(!fragment.is_compiled(), error_type::shader_not_compiled);
    THROW_IF(!geometry.is_compiled(), error_type::shader_not_compiled);
    GLuint id = glCreateProgram();
    THROW_IF(id == 0, error_type::program_not_created);
    handle_.reset(id);
    shaders_.insert(std::make_pair(shader_index::vertex,std::move(vertex)));
    shaders_.insert(std::make_pair(shader_index::fragment,std::move(fragment)));
    shaders_.insert(std::make_pair(shader_index::geometry,std::move(geometry)));
    glAttachShader(handle_.get(), shaders_.at(shader_index::vertex).get());
    glAttachShader(handle_.get(), shaders_.at(shader_index::fragment).get());
    glAttachShader(handle_.get(), shaders_.at(shader_index::geometry).get());
}

void program::attach_vertex_shader(shader&& vertex_shader)
{
    THROW_IF(linked_, error_type::program_already_linked);
    THROW_IF(!vertex_shader.is_compiled(), error_type::shader_not_compiled);
    insert(std::move(vertex_shader),shader_index::vertex);
}

void program::attach_fragment_shader(shader&& fragment_shader)
{
    THROW_IF(linked_, error_type::program_already_linked);
    THROW_IF(!fragment_shader.is_compiled(), error_type::shader_not_compiled);
    insert(std::move(fragment_shader),shader_index::fragment);
}

void program::attach_geometry_shader(shader&& geometry_shader)
{
    THROW_IF(linked_, error_type::program_already_linked);
    THROW_IF(!geometry_shader.is_compiled(), error_type::shader_not_compiled);
    insert(std::move(geometry_shader),shader_index::geometry);
}

GLuint program::uniform_location(const std::string& name)
{
    auto iter = uniform_loc_.find(name);
    if (iter != uniform_loc_.end()) {
        return iter->second;
    } else {
        GLint loc = glGetUniformLocation(handle_.get(), name.c_str());
        THROW_IF(loc < 0, error_type::uniform_not_found);
        uniform_loc_.insert(std::make_pair(name, loc));
        return loc;
    }
}

void program::insert(shader sh, int index)
{
    auto iter = shaders_.find(index);
    if (iter != shaders_.end())
        iter->second = std::move(sh);
    else
        shaders_.insert(std::make_pair(index, std::move(sh)));
}

void program::bind()
{
    THROW_IF(!is_linked(), error_type::program_not_linked);
    glUseProgram(handle_.get());
}

void program::unbind()
{
    THROW_IF(!is_linked(), error_type::program_not_linked);
    glUseProgram(handle_.get());
}

bool program::is_linked() const
{
    return linked_;
}

void program::link()
{
    GLint result = link_program(handle_.get());
    if (result == GL_FALSE)
        std::cerr << get_link_log(handle_.get()) << std::endl;
    THROW_IF(result == GL_FALSE, error_type::program_not_linked);
    linked_ = true;
}

void program::set_attribute(GLuint index, const std::string& name)
{
    THROW_IF(is_linked(), error_type::program_already_linked);
    glBindAttribLocation(handle_.get(), index, name.c_str());
}

void program::set_frag_data_location(GLuint index, const std::string& name)
{
    THROW_IF(is_linked(), error_type::program_already_linked);
    glBindFragDataLocation(handle_.get(), index, name.c_str());
}

GLuint program::get() const
{
    return handle_.get();
}

std::string program::uniforms()
{
    THROW_IF(!is_linked(), error_type::program_not_linked);
    GLint max_length;
    GLint n;
    glGetProgramiv(handle_.get(), GL_ACTIVE_UNIFORMS, &n);
    glGetProgramiv(handle_.get(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
    GLint written, size, location;
    GLenum type;
    std::string buffer(max_length, ' ');
    std::ostringstream ss;
    ss << " Index | Name\n";
    ss << "---------------------------------------------\n";
    for (int i = 0; i < n; ++i) {
        glGetActiveUniform(handle_.get(), i, max_length, &written, &size, &type,
                           &buffer[0]);
        location = glGetUniformLocation(handle_.get(), buffer.c_str());
        ss << location << " | " << buffer.c_str() << std::endl;
    }
    return ss.str();
}

std::string program::attributes()
{
    THROW_IF(!is_linked(), error_type::program_not_linked);
    GLint max_length;
    GLint n;
    glGetProgramiv(handle_.get(), GL_ACTIVE_ATTRIBUTES, &n);
    glGetProgramiv(handle_.get(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
    GLint written, size, location;
    GLenum type;
    std::string buffer(max_length, ' ');
    std::ostringstream ss;
    ss << " Index | Name\n";
    ss << "---------------------------------------------\n";
    for (int i = 0; i < n; ++i) {
        glGetActiveAttrib(handle_.get(), i, max_length, &written, &size, &type,
                          &buffer[0]);
        location = glGetAttribLocation(handle_.get(), buffer.c_str());
        ss << location << " | " << buffer.c_str() << std::endl;
    }
    return ss.str();
}

void program::set_uniform(const std::string& name, const glm::mat4& matrix)
{
    GLint loc = uniform_location(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &matrix[0][0]);
}

void program::set_uniform(const std::string& name, const glm::mat3& matrix)
{
    GLint loc = uniform_location(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &matrix[0][0]);
}

void program::set_uniform(const std::string& name, const glm::vec3& vec)
{
    GLint loc = uniform_location(name);
    glUniform3fv(loc, 1, &vec[0]);
}

void program::set_uniform(const std::string& name, const glm::vec4& vec)
{
    GLint loc = uniform_location(name);
    glUniform4fv(loc, 1, &vec[0]);
}

void program::set_uniform(const std::string& name, int value)
{
    GLint loc = uniform_location(name);
    glUniform1i(loc, value);
}

void program::set_uniform(const std::string& name, float value)
{
    GLint loc = uniform_location(name);
    glUniform1f(loc, value);
}

void program::set_uniform(const std::string& name, bool value)
{
    GLint loc = uniform_location(name);
    glUniform1i(loc, value ? 1 : 0);
}

} // end of glrfw namespace
