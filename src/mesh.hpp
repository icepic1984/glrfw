#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 

namespace glrfw {

class Mesh {
public:
	
	void addTriangle(const glm::vec3& a, const glm::vec3& b,
	                 const glm::vec3& c);
private:

	std::size_t updateVertex(const glm::vec3& a, std::size_t index);
	void updateNeighbors(std::size_t vert_index, std::size_t tri_index);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> vertex_normals;
	std::vector<glm::vec3> face_normals;
	std::vector<glm::ivec3> triangles;
	std::unordered_map<std::size_t,std::vector<std::size_t>> neighbors;
	
};

}

#endif 
