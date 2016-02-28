#ifndef HANDLE_HPP
#define HANDLE_HPP

namespace  glrfw {

namespace detail {

struct gl_handle {

    GLuint x_;

    gl_handle(std::nullptr_t = nullptr) : x_(0)
    {
    }

    gl_handle(GLuint x) : x_(x)
    {
    }

    operator GLuint() const
    {
        return x_;
    }

    friend bool operator==(gl_handle x, gl_handle y)
    {
        return x.x_ == y.x_;
    }
    friend bool operator!=(gl_handle x, gl_handle y)
    {
        return x.x_ != y.x_;
    }
};

template <void (*func)(GLuint)> struct gl_deleter {
    typedef gl_handle pointer;

    void operator()(gl_handle p)
    {
        func(p.x_);
    }
};


} // end namespace detail

} // end namespace glrfw

#endif
