#pragma once
#include <glm/glm.hpp>

class Enemy {
public:
	Enemy();

	void Reset(const glm::vec3& spawnPos);
	void TakeDamage(float dmg);
	void Update(float deltaTime, const glm::vec3& playerPos);

	glm::mat4 GetModelMatrix() const;
	glm::vec4 GetColor() const;

	bool IsActive() const { return active; }
	void Deactivate() { active = false; }

	float GetHP() const { return hp; }
	glm::vec3 GetPosition() const { return position; }

private:
	bool active;
	float hp;
	float maxHp;
	glm::vec3 position;
	float speed;
	float shootTimer;
};
