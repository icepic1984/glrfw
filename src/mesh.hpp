#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp> 
#include <glm/ext.hpp>

namespace glrfw {

class mesh {
public:
	
	void add_triangle(const glm::vec3& a, const glm::vec3& b,
	                 const glm::vec3& c);

	void print_vertices();

	void print_triangles();

	void print_neighbors();
	
private:

	std::size_t update_vertex(const glm::vec3& a, std::size_t index);
	void update_neighbors(std::size_t vert_index, std::size_t tri_index);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> vertex_normals;
	std::vector<glm::vec3> face_normals;
	std::vector<glm::ivec3> triangles;
	std::unordered_map<std::size_t,std::vector<std::size_t>> neighbors;
	
};

}

#endif 
