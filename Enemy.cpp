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
	shootTimer = 1.0f + (rand() % 100) / 50.0f; // случайная задержка 1-3 сек
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

	// Движение к игроку
	glm::vec3 direction = playerPos - position;
	float distance = glm::length(direction);

	if (distance > 2.0f) { // не подходим слишком близко
		direction = glm::normalize(direction);
		position += direction * speed * deltaTime;
	}

	// Система стрельбы (пока только таймер)
	shootTimer -= deltaTime;
	if (shootTimer <= 0.0f) {
		// Здесь будет логика стрельбы по игроку
		shootTimer = 2.0f + (rand() % 100) / 100.0f; // 2-3 секунды
	}
}

glm::mat4 Enemy::GetModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	// Простая анимация - покачивание
	float time = position.x + position.z; // уникальное время для каждого врага
	model = glm::rotate(model, sin(time * 2.0f) * 0.1f, glm::vec3(0, 0, 1));

	model = glm::scale(model, glm::vec3(0.6f)); // размер врага
	return model;
}

glm::vec4 Enemy::GetColor() const {
	if (!active) return glm::vec4(0.3f, 0.3f, 0.3f, 0.3f);

	// Цвет зависит от здоровья
	float healthPercent = hp / maxHp;

	// Красно-оранжевый градиент в зависимости от HP
	float r = 1.0f;
	float g = healthPercent * 0.5f;
	float b = healthPercent * 0.2f;

	return glm::vec4(r, g, b, 0.9f);
}