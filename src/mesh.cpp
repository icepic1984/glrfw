#include "mesh.hpp"

namespace glrfw {

void Mesh::addTriangle(const glm::vec3& a, const glm::vec3& b,
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
	index_a = updateVertex(a, index_a);
	index_b = updateVertex(b, index_b);
	index_c = updateVertex(c, index_c);
	triangles.push_back(glm::ivec3(static_cast<int>(index_a),
	                               static_cast<int>(index_b),
	                               static_cast<int>(index_c)));
	std::size_t tri_index = triangles.size() - 1;
	updateNeighbors(index_a, tri_index);
	updateNeighbors(index_b, tri_index);
	updateNeighbors(index_c, tri_index);
}

void Mesh::updateNeighbors(std::size_t vert_index, std::size_t tri_index)
{
	auto iter = neighbors.find(vert_index);
	if (iter != neighbors.end()){
		iter->second.push_back(tri_index);
	} else {
		neighbors.insert(std::make_pair(vert_index, std::vector<std::size_t>({tri_index})));
	}
}

std::size_t Mesh::updateVertex(const glm::vec3& vertex, std::size_t index)
{
	if (index == -1) {
		vertices.push_back(vertex);
		return vertices.size() -1;
	} else {
		return index;
	}
}

}

