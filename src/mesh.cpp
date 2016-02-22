#include "mesh.hpp"
#include <iostream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace glrfw {

void mesh::add_triangle(const glm::vec3& a, const glm::vec3& b,
                       const glm::vec3& c)
{
	std::size_t index_a = -1;
	std::size_t index_b = -1;
	std::size_t index_c = -1;

	for (std::size_t i = 0; i < vertices.size(); ++i) {
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
	triangles.push_back(glm::ivec3(static_cast<int>(index_a),
	                               static_cast<int>(index_b),
	                               static_cast<int>(index_c)));
	std::size_t tri_index = triangles.size() - 1;
	update_neighbors(index_a, tri_index);
	update_neighbors(index_b, tri_index);
	update_neighbors(index_c, tri_index);
}

void mesh::update_neighbors(std::size_t vert_index, std::size_t tri_index)
{
	auto iter = neighbors.find(vert_index);
	if (iter != neighbors.end()){
		iter->second.push_back(tri_index);
	} else {
		neighbors.insert(std::make_pair(vert_index, std::vector<std::size_t>({tri_index})));
	}
}

std::size_t mesh::update_vertex(const glm::vec3& vertex, std::size_t index)
{
	if (index == -1) {
		vertices.push_back(vertex);
		return vertices.size() -1;
	} else {
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
	for (std::size_t i = 0; i < vertices.size(); ++i){
		std::cout << "Vertex: "<<vertices[i]<<std::endl;
		std::cout << "Tringles:" << std::endl;
		auto faces = neighbors.find(i);
		for (auto iter : faces->second){
			std::cout << iter << std::endl;
		}
	}
}

}

