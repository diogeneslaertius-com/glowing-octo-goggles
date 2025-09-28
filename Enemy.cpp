#include "Enemy.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Enemy::Enemy()
	: active(false), hp(100.0f), maxHp(100.0f),
	position(0.0f), speed(2.5f), shootTimer(0.0f)
{
}

void Enemy::Reset(const glm::vec3& spawnPos) {
	active = true;
	hp = maxHp;
	position = spawnPos;
	shootTimer = 1.0f + (rand() % 100) / 50.0f; // ��������� �������� 1-3 ���
}

void Enemy::TakeDamage(float dmg) {
	if (!active) return;

	hp -= dmg;
	if (hp <= 0.0f) {
		hp = 0.0f;
		active = false;
	}
}

void Enemy::Update(float deltaTime, const glm::vec3& playerPos) {
	if (!active) return;

	// �������� � ������
	glm::vec3 direction = playerPos - position;
	float distance = glm::length(direction);

	if (distance > 2.0f) { // �� �������� ������� ������
		direction = glm::normalize(direction);
		position += direction * speed * deltaTime;
	}

	// ������� �������� (���� ������ ������)
	shootTimer -= deltaTime;
	if (shootTimer <= 0.0f) {
		// ����� ����� ������ �������� �� ������
		shootTimer = 2.0f + (rand() % 100) / 100.0f; // 2-3 �������
	}
}

glm::mat4 Enemy::GetModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	// ������� �������� - �����������
	float time = position.x + position.z; // ���������� ����� ��� ������� �����
	model = glm::rotate(model, sin(time * 2.0f) * 0.1f, glm::vec3(0, 0, 1));

	model = glm::scale(model, glm::vec3(0.6f)); // ������ �����
	return model;
}

glm::vec4 Enemy::GetColor() const {
	if (!active) return glm::vec4(0.3f, 0.3f, 0.3f, 0.3f);

	// ���� ������� �� ��������
	float healthPercent = hp / maxHp;

	// ������-��������� �������� � ����������� �� HP
	float r = 1.0f;
	float g = healthPercent * 0.5f;
	float b = healthPercent * 0.2f;

	return glm::vec4(r, g, b, 0.9f);
}