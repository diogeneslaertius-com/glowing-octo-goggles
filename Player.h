#pragma once
#include <glm/glm.hpp>

class Terrain;

class Player {
public:
	Player();

	void Update(float deltaTime, const Terrain& terrain);

	void MoveForward(float value);   // W/S
	void MoveRight(float value);     // A/D
	void Jump();

	const glm::vec3& GetPosition() const;
	void SetPosition(const glm::vec3& pos);

private:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 inputDir;

	float moveSpeed = 6.0f;
	float jumpForce = 8.0f;
	float gravity = 20.0f;

	bool isGrounded = false;

	void ApplyGravity(float deltaTime, const Terrain& terrain);
};
