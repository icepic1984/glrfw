#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <unordered_map>
#include "error.hpp"

namespace glrfw {

class mesh {
public:
    mesh();

    void add_triangle(const glm::vec3& a, const glm::vec3& b,
                      const glm::vec3& c);

    void calculate_normals();

    void print_vertices();

    void print_triangles();

    void print_neighbors();

    void print_normals();

    int update_vertex(const glm::vec3& a, int index);

    void update_neighbors(int vert_index, int tri_index);

    int find_index(const glm::vec3& vertex);

    void centralize();

    std::vector<glm::vec3> vertices;

    std::vector<glm::vec3> vertex_normals;

    std::vector<glm::vec3> face_normals;

    std::vector<glm::ivec3> triangles;

    struct glm_hash {
        size_t operator()(const glm::vec3& k) const
        {
            return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^
                   std::hash<float>()(k.z);
        }

        bool operator()(const glm::vec3& a, const glm::vec3& b) const
        {
            return a == b;
        } 
    };

    std::unordered_map<glm::vec3,int,glm_hash,glm_hash> indices;
    
    std::unordered_map<int, std::vector<int>> neighbors;
};

mesh parse_stl(const std::string& file);

namespace detail {
template <typename T>
static auto test_glm_to_string(int)
    -> decltype(glm::to_string(std::declval<T&>()), std::true_type());

template <typename> static auto test_glm_to_string(...) -> std::false_type;
}

template <typename T>
struct has_glm_to_string : decltype(detail::test_glm_to_string<T>(0)) {
};

template <typename T, typename = std::enable_if_t<has_glm_to_string<T>::value>>
inline std::ostream& operator<<(std::ostream& out, const T& t)
{
    out << glm::to_string(t);
    return out;
}
}
#endif
