
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>

class Beam {
public:
	Beam(const glm::vec3& origin, const glm::vec3& direction,
		float maxLength, float duration, const glm::vec3& color = glm::vec3(1.0f, 0.2f, 1.0f));
	~Beam();

	void Update(float deltaTime);
	void Draw();

	bool IsAlive() const { return lifetime > 0.0f; }
	glm::mat4 GetModelMatrix() const;
	glm::vec3 GetColor() const;

private:
	glm::vec3 startPos;
	glm::vec3 endPos;
	glm::vec3 baseColor;
	float maxLifetime;
	float lifetime;
	float intensity;
	float width;

	// OpenGL данные
	unsigned int VAO, VBO;
	std::vector<float> vertices;

	void setupMesh();
	void updateMesh();
};
