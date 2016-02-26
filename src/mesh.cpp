#include "mesh.hpp"
#include <iostream>
#include <fstream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "helper.hpp"

namespace glrfw {

mesh::mesh()
    : vertices(std::vector<glm::vec3>()),
      vertex_normals(std::vector<glm::vec3>()),
      face_normals(std::vector<glm::vec3>()),
      triangles(std::vector<glm::ivec3>()),
      neighbors(std::unordered_map<int, std::vector<int>>())
{
}

void mesh::add_triangle(const glm::vec3& a, const glm::vec3& b,
                        const glm::vec3& c)
{
    int index_a = -1;
    int index_b = -1;
    int index_c = -1;

    for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
        if (vertices[i] == a)
            index_a = i;
        if (vertices[i] == b)
            index_b = i;
        if (vertices[i] == c)
            index_c = i;
    }
    index_a = update_vertex(a, index_a);
    index_b = update_vertex(b, index_b);
    index_c = update_vertex(c, index_c);
    triangles.push_back(glm::ivec3(index_a, index_b, index_c));
    glm::vec3 normal(glm::normalize(glm::cross((b - a), (c - a))));
    face_normals.push_back(normal);

    int tri_index = static_cast<int>(triangles.size()) - 1;
    update_neighbors(index_a, tri_index);
    update_neighbors(index_b, tri_index);
    update_neighbors(index_c, tri_index);
}
void mesh::calculate_normals()
{
    vertex_normals = std::vector<glm::vec3>(vertices.size());
    for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
        auto tri_indices = neighbors[i];
        glm::vec3 temp(0, 0, 0);
        for (auto iter : tri_indices) {
            temp += face_normals[iter];
        }
        vertex_normals[i] = temp / tri_indices.size();
    }
}

void mesh::update_neighbors(int vert_index, int tri_index)
{
    auto iter = neighbors.find(vert_index);
    if (iter != neighbors.end()) {
        iter->second.push_back(tri_index);
    }
    else {
        neighbors.insert(
            std::make_pair(vert_index, std::vector<int>({tri_index})));
    }
}

int mesh::update_vertex(const glm::vec3& vertex, int index)
{
    if (index == -1) {
        vertices.push_back(vertex);
        return static_cast<int>(vertices.size()) - 1;
    }
    else {
        return index;
    }
}

void mesh::print_vertices()
{
    for (auto iter : vertices) {
        std::cout << iter << std::endl;
    }
}

void mesh::print_triangles()
{
    for (auto iter : triangles) {
        std::cout << iter << std::endl;
    }
}

void mesh::print_neighbors()
{
    for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
        std::cout << "Vertex: " << vertices[i] << std::endl;
        std::cout << "Tringles:" << std::endl;
        auto faces = neighbors.find(i);
        for (auto iter : faces->second) {
            std::cout << iter << std::endl;
        }
    }
}

mesh parse_stl(const std::string& file)
{
    std::ifstream stl(file, std::ios::in | std::ios::binary);
    mesh mesh;
    if (stl.is_open()) {
        // skip header
        stl.seekg(80, std::ifstream::beg);
        uint32_t num_tri = 0;
        stl.read(reinterpret_cast<char*>(&num_tri), sizeof(uint32_t));
        for (uint32_t i = 0; i < num_tri; i++) {
            // skip face normals
            stl.seekg(3 * sizeof(float), std::ifstream::cur);
            glm::vec3 a, b, c;
            stl.read(reinterpret_cast<char*>(&a), sizeof(a));
            stl.read(reinterpret_cast<char*>(&b), sizeof(b));
            stl.read(reinterpret_cast<char*>(&c), sizeof(c));
            stl.seekg(sizeof(uint16_t), std::ifstream::cur);
            mesh.add_triangle(a, b, c);
            if (i % 10000 == 0)
                std::cout << i << " from: " << num_tri << std::endl;
        }
    }
    else {
	    THROW_IF(!stl.is_open(),error_type::file_not_found);
    }
    mesh.calculate_normals();
    return mesh;
}
}
