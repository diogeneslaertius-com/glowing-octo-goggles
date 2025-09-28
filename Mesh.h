#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <gl/glew.h>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void Draw();

private:
	unsigned int VBO, EBO;
	void setupMesh();
};
