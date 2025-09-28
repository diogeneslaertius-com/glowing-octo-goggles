#include "Player.h"
#include "Terrain.h"
#include <algorithm>

Player::Player()
	: position(0.0f, 2.0f, 0.0f), velocity(0.0f), inputDir(0.0f),
	moveSpeed(6.0f), jumpForce(8.0f), gravity(20.0f), isGrounded(false)
{
}

void Player::Update(float deltaTime, const Terrain& terrain) {
	// ��������� ������� �� ������ �����
	if (glm::length(inputDir) > 0.0f) {
		glm::vec3 normalizedInput = glm::normalize(inputDir);
		velocity.x = normalizedInput.x * moveSpeed;
		velocity.z = normalizedInput.z * moveSpeed;
	}
	else {
		// ������ ��� ���������� �����
		velocity.x *= 0.8f;
		velocity.z *= 0.8f;
	}

	// ��������� ����������
	ApplyGravity(deltaTime, terrain);

	// ��������� �������
	position += velocity * deltaTime;

	// ���������� ����������� ����� ��� ���������� �����
	inputDir = glm::vec3(0.0f);
}

void Player::MoveForward(float value) {
	inputDir.z += value;
}

void Player::MoveRight(float value) {
	inputDir.x += value;
}

void Player::Jump() {
	if (isGrounded) {
		velocity.y = jumpForce;
		isGrounded = false;
	}
}

const glm::vec3& Player::GetPosition() const {
	return position;
}

void Player::SetPosition(const glm::vec3& pos) {
	position = pos;
}

void Player::ApplyGravity(float deltaTime, const Terrain& terrain) {
	// ��������� ����������
	velocity.y -= gravity * deltaTime;

	// ��������� �������� � ������
	float groundHeight = terrain.GetHeightAt(position.x, position.z);

	if (position.y <= groundHeight + 0.1f) { // ��������� offset ��� ��������
		position.y = groundHeight + 0.1f;
		velocity.y = 0.0f;
		isGrounded = true;
	}
	else {
		isGrounded = false;
	}
}