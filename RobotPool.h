#pragma once
#pragma once
#include "AnimatedRobot.h"
#include <vector>

class RobotPool {
public:
	RobotPool(size_t size) { robots.resize(size); }

	// Спавн робота
	AnimatedRobot* Spawn(const glm::vec3& pos, RobotType type = BASIC_ROBOT);

	// Обновление всех роботов
	void UpdateAll(float deltaTime, const glm::vec3& playerPos);

	// Рендеринг всех роботов (опционально)
	void DrawAll();

	// Спавн волны роботов по кругу
	void SpawnWaveCircle(int count, const glm::vec3& center, float radius,
		float hunterChance = 0.2f, float jitter = 0.5f);

	// Получение количества активных роботов
	int GetActiveCount() const;

	// Доступ к роботам
	std::vector<AnimatedRobot>& GetRobots() { return robots; }
	const std::vector<AnimatedRobot>& GetRobots() const { return robots; }

	// Инициализация и очистка
	static bool Initialize(const std::string& robotModelPath);
	static void Cleanup();

private:
	std::vector<AnimatedRobot> robots;
};
