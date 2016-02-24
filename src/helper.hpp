#ifndef HELPER_HPP
#define HELPER_HPP
#include <sstream>

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
