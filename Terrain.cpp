#include "Terrain.h"
#include <vector>

Terrain::Terrain(float size, float y) : terrainSize(size), terrainY(y) {
	setupMesh(size, y);
}

Terrain::~Terrain() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Terrain::setupMesh(float size, float y) {
	// —оздаем простую плоскую поверхность
	std::vector<float> vertices = {
		// ѕозиции (x, y, z)
		-size, y, -size,  // 0: левый задний
		 size, y, -size,  // 1: правый задний
		 size, y,  size,  // 2: правый передний
		-size, y,  size   // 3: левый передний
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,  // первый треугольник
		2, 3, 0   // второй треугольник
	};

	indexCount = indices.size();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
		vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		indices.data(), GL_STATIC_DRAW);

	// ѕозици€ (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Terrain::Draw() const {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

float Terrain::GetHeightAt(float x, float z) const {
	// ƒл€ плоской поверхности просто возвращаем Y координату
	// ¬ будущем здесь можно добавить интерпол€цию высот дл€ сложной геометрии
	return terrainY;
}