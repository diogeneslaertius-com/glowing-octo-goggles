
#include "RobotPool.h"
#include <glm/gtc/constants.hpp>
#include <random>

bool RobotPool::Initialize(const std::string& robotModelPath) {
	return AnimatedRobot::LoadRobotModel(robotModelPath);
}

void RobotPool::Cleanup() {
	AnimatedRobot::CleanupModels();
}

AnimatedRobot* RobotPool::Spawn(const glm::vec3& pos, RobotType type) {
	for (auto& robot : robots) {
		if (!robot.IsActive()) {
			robot.Reset(pos, type);
			return &robot;
		}
	}
	return nullptr; // пул заполнен
}

void RobotPool::UpdateAll(float deltaTime, const glm::vec3& playerPos) {
	for (auto& robot : robots) {
		if (!robot.IsActive()) continue;
		robot.Update(deltaTime, playerPos);
	}
}

void RobotPool::DrawAll() {
	for (const auto& robot : robots) {
		if (!robot.IsActive()) continue;
		robot.Draw();
	}
}

void RobotPool::SpawnWaveCircle(int count, const glm::vec3& center, float radius,
							   float hunterChance, float jitter) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> jitterDis(-jitter, jitter);
	std::uniform_real_distribution<float> chanceDis(0.0f, 1.0f);

	for (int i = 0; i < count; i++) {
		float angle = (float)i / count * 2.0f * glm::pi<float>();
		float actualRadius = radius + jitterDis(gen);
		angle += jitterDis(gen) * 0.5f;

		glm::vec3 spawnPos = center + glm::vec3(
			cos(angle) * actualRadius,
			0.0f,
			sin(angle) * actualRadius
		);

		// Определяем тип робота
		RobotType type = BASIC_ROBOT;
		if (chanceDis(gen) < hunterChance) {
			type = TEMPORAL_HUNTER_ROBOT;
		}

		Spawn(spawnPos, type);
	}
}

int RobotPool::GetActiveCount() const {
	int count = 0;
	for (const auto& robot : robots) {
		if (robot.IsActive()) count++;
	}
	return count;
}